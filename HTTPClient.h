#pragma once

#include <string>
#include <memory>

#include "HTTPRequest.h"
#include "SSLCertificatePolicy.hpp"

class HTTPClient
{
public:
	HTTPClient();

    // 异步请求
    void startAsyncRequest(std::shared_ptr<HTTPRequest> request);
    
    // 同步请求
    std::shared_ptr<HTTPResponse> startSyncRequest(std::shared_ptr<HTTPRequest> request);
    
    //	void cancelRequest(std::shared_ptr<HTTPRequest> request);

	~HTTPClient();

    std::shared_ptr<SSLCertificatePolicy> getSSLCertPolicy() const;
    void setSSLCertPolicy(std::shared_ptr<SSLCertificatePolicy> sslCertPolicy);
    
private:
    void *_priv_data;

public:
	static HTTPClient *client();
};

