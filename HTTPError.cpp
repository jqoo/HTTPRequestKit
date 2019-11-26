//
//  HTTPError.cpp
//  pubkey-pin
//
//  Created by jqoo on 2016/12/21.
//  Copyright © 2016年 OWASP. All rights reserved.
//

#include "HTTPError.hpp"

using namespace std;

HTTPError::HTTPError(HTTPError_Code code, const std::string &error_string)
: _code(code), _error_string(error_string) {
    
}

HTTPError::HTTPError_Code HTTPError::getCode() const {
    return _code;
}

const std::string HTTPError::getString() const {
    return _error_string;
}

HTTPError HTTPError::noError() {
    return HTTPError(HTTPError_Code_OK, "");
}
