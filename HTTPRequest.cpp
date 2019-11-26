#include "HTTPRequest.h"

static const int HTTPRequest_Timeout_Default = 30;

HTTPRequest::HTTPRequest(HTTPRequest::HTTP_Method method, const std::string &url)
: _url(url), _method(method) {
    _timeout = HTTPRequest_Timeout_Default;
}

HTTPRequest::~HTTPRequest() {
}

const std::string& HTTPRequest::getUrl() const {
    return _url;
}

const std::map<std::string, std::string>& HTTPRequest::getAllHeaders() const {
    return _headerMapping;
}

const std::string HTTPRequest::getHeaderField(const std::string &name) {
    return _headerMapping[name];
}

void HTTPRequest::setHeaderField(const std::string &name, const std::string &value) {
    _headerMapping[name] = value;
}

HTTPRequest::HTTP_Method HTTPRequest::getMethod() const {
    return _method;
}

void HTTPRequest::setResponseCallback(HTTPResponseCallback callback) {
    //_callback = callback;
}

const HTTPRequest::HTTPResponseCallback &HTTPRequest::getResponseCallback() const {
   // return _callback;
    return NULL;
}

void HTTPRequest::setTimeout(int timeout) {
    if (timeout < 0) {
        timeout = 0;
    }
    _timeout = timeout;
}

int HTTPRequest::getTimeout() const {
    return _timeout;
}

void HTTPRequest::setOpaque(void *opaque) {
    _opaque = opaque;
}

void *HTTPRequest::getOpaque() const {
    return _opaque;
}
