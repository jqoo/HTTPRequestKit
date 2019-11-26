#pragma once

#include "HTTPResponse.h"

#include <string>
#include <vector>
#include <map>

//class HTTPRequest;
//void (*HTTPRequestCallback)(HTTPRequest *request, std::vector<unsigned char>& responseData);

class HTTPRequest
{
public:
	typedef enum {
		HTTP_Method_GET,
		HTTP_Method_POST,
		HTTP_Method_PUT,
		HTTP_Method_HEAD,
        HTTP_Method_DELETE
	} HTTP_Method;

    typedef std::function<void (std::shared_ptr<HTTPRequest> request, std::shared_ptr<HTTPResponse> response)> HTTPResponseCallback;

public:
	HTTPRequest(HTTP_Method method, const std::string &url);

    const std::string& getUrl() const;
    HTTP_Method getMethod() const;

    const std::map<std::string, std::string>& getAllHeaders() const;
    const std::string getHeaderField(const std::string &name);
    void setHeaderField(const std::string &name, const std::string &value);

    // 异步调用可使用该回调
    void setResponseCallback(HTTPResponseCallback callback);
    const HTTPResponseCallback &getResponseCallback() const;

    void setTimeout(int timeout);
    int getTimeout() const;

    void setOpaque(void *opaque);
    void *getOpaque() const;

	~HTTPRequest();

private:
    std::string _url;
    HTTP_Method _method;
    //HTTPResponseCallback _callback;
    int _timeout;
    void *_opaque;
    std::map<std::string, std::string> _headerMapping;
};
