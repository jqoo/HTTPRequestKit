//
//  SSLCertificatePolicy.hpp
//  pubkey-pin
//
//  Created by jqoo on 2016/12/25.
//  Copyright © 2016年 OWASP. All rights reserved.
//

#ifndef SSLCertificatePolicy_hpp
#define SSLCertificatePolicy_hpp

#include <openssl/x509_vfy.h>

class HTTPRequest;

class SSLCertificatePolicy {
public:
    virtual bool veryfy(HTTPRequest *request, X509_STORE_CTX *ctx);
};

#endif /* SSLCertificatePolicy_hpp */
