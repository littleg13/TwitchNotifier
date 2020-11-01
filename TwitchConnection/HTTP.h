#ifndef HTTP_H
#define HTTP_H

#include <unordered_map>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <synchapi.h>
#include <stdexcept>
#include <iostream>
#include <limits>
#include <sstream>
#include <algorithm>
#include "json11.hpp"

#define RECV_BUF_SIZE 8192

struct HTTPResponse {
    enum STATUS_CODES {
        OK = 200,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        SWITCHING_PROTOCOLS = 101,
        FORBIDDEN = 403,
        BAD_REQUEST = 400
    };
    enum HTTP_TYPE {
        GET,
        POST,
        OPTIONS,
        RESPONSE
    };
    static std::unordered_map<std::string, HTTP_TYPE> httpRequests;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> params;
    HTTP_TYPE type = HTTP_TYPE::RESPONSE;
    int status_code = 0;
    std::string request = "";
    std::string status_msg = "";
    json11::Json jsonContent = nullptr;
    std::string content = "";

    HTTPResponse(std::string s, int dataLength){
        std::stringstream ss;
        ss.str(s);
        std::string value;
        ss >> value;
        //See whether it is a request or response
        if(httpRequests.find(value) != httpRequests.end()){
            type = httpRequests[value];
            ss >> request;
            extractParams(request);
        }
        else{
            ss >> status_code;
            ss >> status_msg;
        }
        //Read all headers (Remove '\r' character cause windows is fucking stupid)
        std::getline(ss.ignore(INT_MAX, '\n'), value);
        value.pop_back();
        while(value.size()){
            int pos = value.find(":");
            headers.insert({value.substr(0, pos), value.substr(pos + 2)}); // + 2 to accout for ": "
            std::getline(ss, value);
            value.pop_back();
        }
        //Shove the rest into the content
        while(!ss.eof()){
            std::getline(ss, value);
            content += value;
        }
        try{
            if(headers["Content-Type"].find("application/json") != std::string::npos){
                std::string err;
                jsonContent = json11::Json::parse(content, err);
            }
        } catch(std::out_of_range &e){}
    }

    void extractParams(std::string s){
        std::size_t pos = s.find("?");
        if(pos!=std::string::npos){
            request = s.substr(0, pos);
            std::string paramString = s.substr(pos + 1);
            s.erase(0, pos + 1);
            pos = paramString.size();
            while(pos!=std::string::npos){
                int eqPos = paramString.find("=");
                params.insert({paramString.substr(0, eqPos), paramString.substr(eqPos + 1)});
                paramString.erase(0, pos + 1);
                pos = paramString.find("&");
            }
        }
    }
};

class HTTP {
    public:
        static HTTPResponse* ssl_recv(SSL* ssl, SOCKET Socket);
        static HTTPResponse* http_recv(SOCKET Socket);
        static HTTPResponse* ssl_GET(SSL* ssl, SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static HTTPResponse* http_GET(SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static HTTPResponse* ssl_POST(SSL* ssl, SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static HTTPResponse* http_POST(SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static void http_response(SOCKET Socket, int status_code, std::string status_msg, std::string* headers, int numHeaders, std::string body);
};
#endif