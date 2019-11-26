#include "HTTPResponse.h"

#include <sstream>
#include <string.h>

using namespace std;

HTTPResponse::~HTTPResponse() {
}

HTTPResponse::HTTPResponse(int httpCode, std::shared_ptr<std::vector<unsigned char> > data, const std::vector<unsigned char> & headerData)
: _data(data), _httpCode(httpCode) {
    parseResponseHeader(headerData, _headerMapping);
}

HTTPResponse::HTTPResponse(int httpCode, std::shared_ptr<std::vector<unsigned char> > data)
: _data(data), _httpCode(httpCode) {
}

int HTTPResponse::getHTTPCode() const {
    return _httpCode;
}

const std::shared_ptr<std::vector<unsigned char> > & HTTPResponse::getData() const {
    return _data;
}

const std::string HTTPResponse::getHeaderField(const std::string &name) {
    return _headerMapping[name];
}

const std::shared_ptr<HTTPError> &HTTPResponse::getError() const {
    return _error;
}

void HTTPResponse::setError(std::shared_ptr<HTTPError> error) {
    _error = error;
}

void HTTPResponse::parseResponseHeader(const std::vector<unsigned char> & header_data, map<string, string> &headerMapping) {
    if (!header_data.empty()) {
        char *data = (char *)header_data.data();
        istringstream iss(data);
        const int BufLen = 1024;
        char buf[BufLen];
        while (!iss.eof()) {
            iss.getline(buf, BufLen);

            char *key = NULL;
            char *value = NULL;
            char *p = buf;
            while (*p) {
                if (!key && !isspace(*p)) {
                    key = p;
                }
                else if (key && *p == ':') {
                    value = p + 1;
                    *p-- = '\0';
                    while (p != key && isspace(*p)) {
                        *p-- = '\0';
                    }
                    break;
                }
                p++;
            }
            if (value) {
                while (*value && isspace(*value)) {
                    value++;
                }
                if (*value) {
                    p = value + strlen(value) - 1;
                    while (p != value && isspace(*p)) {
                        *p-- = '\0';
                    }
                }
            }
            if (key && value) {
                headerMapping[key] = value;
            }
        }
    }
}
