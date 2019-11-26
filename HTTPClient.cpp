#include "HTTPClient.h"

#include <thread>
#include <queue>
#include <mutex>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <iostream>

#include "HTTPError.hpp"

#include "curl/curl.h"

#include <openssl/x509v3.h>
#include <openssl/x509_vfy.h>
#include <openssl/crypto.h>
#include <openssl/lhash.h>
#include <openssl/objects.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pkcs12.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>

extern void logFormat(char *fs, ...);

#ifndef WIN32

#  include <unistd.h>

#else

//#pragma comment(lib,"libcurl.a")

#endif

using namespace std;

#define MAX 10
// runloop最长等待时间, in seconds
#define MAX_LOOP_TIME 0.2

struct HTTPClientImpl;

struct RequestInfo {
    shared_ptr<vector<unsigned char> > response_data;
    shared_ptr<vector<unsigned char> > response_header_data;
    int http_code;
    CURLcode curl_code;
    CURL *curl_handler;
    shared_ptr<HTTPRequest> request;
    struct curl_slist *http_header;
    HTTPClientImpl *client_impl;

    ~RequestInfo() {
        if (http_header) {
            curl_slist_free_all(http_header);
            http_header = NULL;
        }
        curl_handler = NULL;
        client_impl = NULL;
    }

    RequestInfo(CURL *curl, shared_ptr<HTTPRequest> req)
    : curl_handler(curl), request(req) {
        http_header = NULL;
        response_data = make_shared<vector<unsigned char> >();
        response_header_data = make_shared<vector<unsigned char> >();
    }

    void appendResponseData(unsigned char *data, size_t length) {
        logFormat("http response data append length :%d", (int)length);

        response_data->insert(response_data->end(), data, data + length );  // added by shijianchun sub -
    }

    void appendResponseHeaderData(unsigned char *data, size_t length) {
        logFormat("http response header append length :%d", (int)length);

        response_header_data->insert(response_header_data->end(), data, data + length);
    }

    inline HTTPError::HTTPError_Code convertCurlCode2HTTPErrorCode(CURLcode code) const {
        return (HTTPError::HTTPError_Code)code;
    }

    std::shared_ptr<HTTPResponse> makeResponse() const {
        std::shared_ptr<HTTPResponse> response = make_shared<HTTPResponse>(http_code, response_data, *response_header_data);
        if (curl_code != CURLE_OK) {
            const char *errString = curl_easy_strerror(curl_code);
            string str;
            HTTPError::HTTPError_Code code = convertCurlCode2HTTPErrorCode(curl_code);
            char sbuf[12];
            snprintf(sbuf, 12, "%d", code);
            if (errString) {
                str = "(" + string(sbuf) + ") - " + errString + " <" + request->getUrl() + ">";
            }
            else {
                str = "(" + string(sbuf) + ") - unknow error" + " <" + request->getUrl() + ">";
            }
            response->setError(make_shared<HTTPError>(code, str));
        }
        return response;
    }
};

struct HTTPClientImpl {
	thread *runloop_thr;
	CURLM *curl_m;
	bool active;
    mutex waiting_queue_mutex;
    queue<std::shared_ptr<HTTPRequest> > waiting_queue;
    map<HTTPRequest *, RequestInfo *> requestMapping;
    std::shared_ptr<SSLCertificatePolicy> sslCertPolicy;

    HTTPClientImpl()/*:waiting_queue_mutex(),waiting_queue()*/ {
		runloop_thr = NULL;
		curl_m = curl_multi_init();
		curl_multi_setopt(curl_m, CURLMOPT_MAXCONNECTS, (long)MAX);
		active = false;
	}

    ~HTTPClientImpl() {
        for (auto iter = requestMapping.begin(); iter != requestMapping.end(); ++iter) {
            if (iter->second) {
                delete iter->second;
                iter->second = NULL;
            }
        }
        if (curl_m) {
            curl_multi_cleanup(curl_m);
            curl_m = NULL;
        }
    }

    void enqueue(std::shared_ptr<HTTPRequest> request) {
        waiting_queue_mutex.lock();
        waiting_queue.push(request);
        waiting_queue_mutex.unlock();
    }

    std::shared_ptr<HTTPRequest> dequeue() {
        std::shared_ptr<HTTPRequest> request;
        waiting_queue_mutex.lock();
        if (!waiting_queue.empty()) {
            request = waiting_queue.front();
            waiting_queue.pop();
        }
        waiting_queue_mutex.unlock();
        return request;
    }

    static void thread_runloop_main(HTTPClientImpl *impl) {
        if (impl) {
            impl->runloop_main();
        }
    }

    void startRunLoop() {
		if (!runloop_thr) {
            active = true;
            runloop_thr = new thread(thread_runloop_main, this);
			runloop_thr->detach();
		}
	}


	void stopRunLoop() {
        if (runloop_thr) {
            if (active) {
                active = false;
                runloop_thr->join();
            }
            delete runloop_thr;
            runloop_thr = NULL;
        }
	}
	#ifdef  _WIN32
    static string ws2s(const wstring& ws)
    {
        logFormat("ws2s --- before");

        string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
        setlocale(LC_ALL, "chs");
        const wchar_t* _Source = ws.c_str();
        size_t _Dsize = 2 * ws.size() + 1;
        char *_Dest = new char[_Dsize];
        memset(_Dest,0,_Dsize);
        wcstombs(_Dest,_Source,_Dsize);
        string result = _Dest;
        delete []_Dest;
        setlocale(LC_ALL, curLocale.c_str());
        logFormat("ws2s --- after");
        return result;

    }


    static string DecodeUtf8(char *in,int nSize)
    {
          if(nSize == 0)
                return "";

        logFormat("DecodeUtf8");

        wstring s(nSize+1, (' '));
        int len = ::MultiByteToWideChar(CP_UTF8, 0,
            in, nSize,
            &s[0], (int)s.length());
        s.resize(len);
        return ws2s(s);
    }
     #endif // _WINDOWS

    static size_t write_callback(unsigned char *buffer, size_t size, size_t nitems, void *userdata) {
        RequestInfo *ri = (RequestInfo *)userdata;

        char* pData = (char*)buffer;
      /*
        #ifdef  _WIN32
	    string strtmp = DecodeUtf8(pData,nitems);
        ri->appendResponseData((unsigned char *)strtmp.c_str(),strtmp.size()) ;//(buffer, size * nitems);
        #else
          ri->appendResponseData(buffer, size * nitems); ;//
        #endif
       */
        ri->appendResponseData(buffer, size * nitems); ;//
        return size * nitems;
    }

    static size_t header_callback(unsigned char *buffer, size_t size, size_t nitems, void *userdata) {
        RequestInfo *ri = (RequestInfo *)userdata;
        char* pData = (char*)buffer;
       /* #if _WIN32
	    string strtmp = DecodeUtf8(pData,nitems);
        ri->appendResponseHeaderData((unsigned char *)strtmp.c_str(),strtmp.size()) ;//(buffer, size * nitems);
        #else
          ri->appendResponseHeaderData(buffer, size * nitems); ;//
        #endif // _WINDOWS
        */
        ri->appendResponseHeaderData(buffer, size * nitems); ;//
        return size * nitems;
    }

    static int cert_veryfy_callback(X509_STORE_CTX *ctx, void *parm) {
        RequestInfo *ri = (RequestInfo *)parm;
        SSLCertificatePolicy *policy = ri->client_impl->sslCertPolicy.get();
        if (policy) {
            return policy->veryfy(ri->request.get(), ctx) ? 1:0;
        }
        else {
            return X509_verify_cert(ctx);
        }
    }

    static CURLcode sslctxfun(CURL *, void *sslctx, void *parm) {
        logFormat("sslctxfun");

        SSL_CTX *ctx = (SSL_CTX *) sslctx;
        SSL_CTX_set_cert_verify_callback(ctx, cert_veryfy_callback, parm);
        return CURLE_OK;
    }

    RequestInfo *createRequestHandler(std::shared_ptr<HTTPRequest> request) {
        CURL *handler = curl_easy_init();

        RequestInfo *ri = new RequestInfo(handler, request);


        ri->client_impl = this;

        switch (request->getMethod()) {
            case HTTPRequest::HTTP_Method_POST:
                curl_easy_setopt(handler, CURLOPT_POST, 1);
                curl_easy_setopt(handler, CURLOPT_POSTFIELDSIZE, 0);
                break;

            case HTTPRequest::HTTP_Method_PUT:
                curl_easy_setopt(handler, CURLOPT_PUT, 1);
                break;

            case HTTPRequest::HTTP_Method_DELETE:
                curl_easy_setopt(handler, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;

            case HTTPRequest::HTTP_Method_HEAD:
                curl_easy_setopt(handler, CURLOPT_NOBODY, 1);
                break;

            default:
                curl_easy_setopt(handler, CURLOPT_HTTPGET, 1);
                break;
        }

        curl_easy_setopt(handler, CURLOPT_URL, request->getUrl().c_str());
        curl_easy_setopt(handler, CURLOPT_PRIVATE, ri);
        curl_easy_setopt(handler, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(handler, CURLOPT_WRITEDATA, ri);
        curl_easy_setopt(handler, CURLOPT_HEADERFUNCTION, header_callback);
        curl_easy_setopt(handler, CURLOPT_HEADERDATA, ri);
        curl_easy_setopt(handler, CURLOPT_CONNECTTIMEOUT, 3);
        curl_easy_setopt(handler, CURLOPT_TIMEOUT, request->getTimeout());

        curl_easy_setopt(handler, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(handler, CURLOPT_SSL_VERIFYHOST, 1L);
        curl_easy_setopt(handler, CURLOPT_CAINFO, "cacert.pem" );
     //   curl_easy_setopt(handler, CURLOPT_SSL_CTX_FUNCTION, sslctxfun);
     //   curl_easy_setopt(handler, CURLOPT_SSL_CTX_DATA, ri);

        curl_easy_setopt(handler, CURLOPT_FRESH_CONNECT, 1);

        auto allHeaders = request->getAllHeaders();
        if (!allHeaders.empty()) {
            for (auto iter = allHeaders.begin(); iter != allHeaders.end(); ++iter) {
                string line = iter->first + ": " + iter->second;
                ri->http_header = curl_slist_append(ri->http_header, line.c_str());
            }
            if (ri->http_header) {
                curl_easy_setopt(handler, CURLOPT_HTTPHEADER,ri->http_header );
            }
        }
        curl_easy_setopt(handler,CURLOPT_FOLLOWLOCATION,0);  //shijianchun  ri->http_header
        curl_easy_setopt(handler, CURLOPT_READFUNCTION, NULL);   //shijianchun  ri->http_header

        return ri;
    }

    void addRequestIfNeeded() {
        std::shared_ptr<HTTPRequest> request = dequeue();
        if (request.get()) {
            RequestInfo *ri = createRequestHandler(request);
            curl_multi_add_handle(curl_m, ri->curl_handler);

            requestMapping[request.get()] = ri;
        }
    }

    void finishRequestWithMsg(CURLMsg *msg) {
        CURL *handler = msg->easy_handle;
        RequestInfo *ri = NULL;
        long httpCode = 0;
        curl_easy_getinfo(handler, CURLINFO_PRIVATE, &ri);
        curl_easy_getinfo(handler, CURLINFO_RESPONSE_CODE, &httpCode);

        cout << "url : " << ri->request->getUrl() <<  " http code : " << httpCode << endl;

        ri->http_code = (int)httpCode;
        ri->curl_code = msg->data.result;

        std::shared_ptr<HTTPResponse> response = ri->makeResponse();
        HTTPRequest::HTTPResponseCallback callback = ri->request->getResponseCallback();
        if (callback) {
            callback(ri->request, response);
        }
        requestMapping.erase(ri->request.get());
        curl_multi_remove_handle(curl_m, handler);
        curl_easy_cleanup(handler);

        delete ri;
    }

	void runloop_main() {
		int M, running_handles = -1;
		long L;
		struct timeval T;

		while (active) {
            long loop_time = (long)(MAX_LOOP_TIME * 1000);

            addRequestIfNeeded();

			curl_multi_perform(curl_m, &running_handles);
			if (running_handles) {
                fd_set R, W, E;
                FD_ZERO(&R);
				FD_ZERO(&W);
				FD_ZERO(&E);

				if (curl_multi_fdset(curl_m, &R, &W, &E, &M) || M == -1) {
                    this_thread::sleep_for(chrono::microseconds(loop_time));
				}
				else {
                    // 毫秒
                    if (curl_multi_timeout(curl_m, &L) == CURLM_OK && L > -1 && L < loop_time) {
                        loop_time = L;
                    }

					T.tv_sec = loop_time / 1000;
					T.tv_usec = (loop_time % 1000) * 1000;

					if (0 > select(M + 1, &R, &W, &E, &T)) {
                        cout << "time out" << endl;
					}
				}
			}
			else {
                this_thread::sleep_for(chrono::microseconds(loop_time));
			}

            CURLMsg *msg;
            int msgs_in_queue;
            while ((msg = curl_multi_info_read(curl_m, &msgs_in_queue))) {
                if (msg->msg == CURLMSG_DONE) {
                    finishRequestWithMsg(msg);
                }
                else {
                    fprintf(stderr, "E: CURLMsg (%d)\n", msg->msg);
                }
            }
        }
    }

	static int init_count;

	static void init() {
		if (init_count == 0) {
            OpenSSL_add_all_ciphers();
            OpenSSL_add_all_digests();
//            ERR_load_crypto_strings();
            curl_global_init(CURL_GLOBAL_ALL);
		}
		init_count++;
	}

	static void cleanup() {
		if (init_count > 0) {
			init_count--;
			if (init_count == 0) {
				curl_global_cleanup();
			}
		}
	}
};

#define _impl_ptr ((HTTPClientImpl *)_priv_data)

HTTPClient::HTTPClient() {
	_priv_data = new HTTPClientImpl();
}

HTTPClient *HTTPClient::client() {
	static HTTPClient *instance;
	if (instance == NULL) {
		instance = new HTTPClient();
	}
	return instance;
}

void HTTPClient::startAsyncRequest(std::shared_ptr<HTTPRequest> request) {
    if (!_impl_ptr->active) {
        _impl_ptr->startRunLoop();
    }
    _impl_ptr->enqueue(request);
}

int g_async_count;

#include <condition_variable>

void logFormat(char *fs, ...);

std::shared_ptr<HTTPResponse> HTTPClient::startSyncRequest(std::shared_ptr<HTTPRequest> request) {
    std::shared_ptr<HTTPResponse> resp;
    std::mutex m;
    std::unique_lock<std::mutex> lk(m);
    std::condition_variable cv;

    request->setResponseCallback([&cv, &resp](std::shared_ptr<HTTPRequest> request, std::shared_ptr<HTTPResponse> response){
                                 resp = response;
                                 logFormat("notify");
                                 cv.notify_one();
                                 });

    logFormat("starting request");

    this->startAsyncRequest(request);

    logFormat("waiting");
    cv.wait(lk);
    logFormat("continued");

    return resp;
/*
    logFormat("before g_async_count : %d", g_async_count);
    g_async_count++;
    RequestInfo *ri = _impl_ptr->createRequestHandler(request);
    ri->curl_code = curl_easy_perform(ri->curl_handler);
    long long http_code;
    curl_easy_getinfo(ri->curl_handler, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(ri->curl_handler);  //added
    ri->http_code = (int)http_code;
    std::shared_ptr<HTTPResponse> response = ri->makeResponse();
    delete ri;
    g_async_count--;
    logFormat("after g_async_count : %d", g_async_count);
    return response;
    */
}

std::shared_ptr<SSLCertificatePolicy> HTTPClient::getSSLCertPolicy() const {
    return _impl_ptr->sslCertPolicy;
}

void HTTPClient::setSSLCertPolicy(std::shared_ptr<SSLCertificatePolicy> sslCertPolicy) {
    _impl_ptr->sslCertPolicy = sslCertPolicy;
}

//void HTTPClient::cancelRequest(std::shared_ptr<HTTPRequest> request) {
//    // TODO:
//}

HTTPClient::~HTTPClient() {
    delete _impl_ptr;
}
