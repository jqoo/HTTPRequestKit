#pragma once

#include <memory>
#include <vector>
#include <map>
#include <string>

#include "HTTPError.hpp"

class HTTPResponse
{
public:
    HTTPResponse(int httpCode, std::shared_ptr<std::vector<unsigned char> > data, const std::vector<unsigned char> & headerData);
    HTTPResponse(int httpCode, std::shared_ptr<std::vector<unsigned char> > data);
	~HTTPResponse();

    int getHTTPCode() const;
    const std::shared_ptr<std::vector<unsigned char> > &getData() const;
    const std::string getHeaderField(const std::string &name);
    
    const std::shared_ptr<HTTPError> &getError() const;
    void setError(std::shared_ptr<HTTPError> error);

private:
	std::shared_ptr<std::vector<unsigned char> > _data;
    std::shared_ptr<HTTPError> _error;
	int _httpCode;
    std::map<std::string, std::string> _headerMapping;
    
public:
    static void parseResponseHeader(const std::vector<unsigned char> & header_data, std::map<std::string, std::string> &headerMapping);
};
