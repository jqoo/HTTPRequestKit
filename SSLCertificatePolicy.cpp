//
//  SSLCertificatePolicy.cpp
//  pubkey-pin
//
//  Created by jqoo on 2016/12/25.
//  Copyright © 2016年 OWASP. All rights reserved.
//

#include "SSLCertificatePolicy.hpp"

bool SSLCertificatePolicy::veryfy(HTTPRequest *, X509_STORE_CTX *ctx) {
    return X509_verify_cert(ctx);
}
