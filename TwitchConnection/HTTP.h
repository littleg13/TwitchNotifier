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
#include <algorithm>
#include "json11.hpp"

#define RECV_BUF_SIZE 8192

struct HTTPResponse {
    enum STATUS_CODES {
        OK = 200,
        ACCEPTED = 202,
        SWITCHING_PROTOCOLS = 101
    };
    enum HTTP_TYPE {
        GET,
        POST,
        RESPONSE
    };
    std::unordered_map<std::string, std::string*> httpLookups;
    HTTP_TYPE type;
    int status_code = 0;
    std::string request = "";
    std::string status_msg = "";
    std::string date = "";
    std::string content_type = "";
    std::string content_length_str = "";
    int content_length = 0;
    json11::Json jsonContent = nullptr;
    std::string content = "";
    std::string sec_websocket_accept = "";

    void fixInputs(){
        int pos = status_msg.find(" ");
        if(pos != std::string::npos){
            status_code = stoi(status_msg.substr(0, pos));
            status_msg = status_msg.substr(pos + 1, status_msg.length());
        }
        if((pos = content_type.find(";")) != std::string::npos){
            content_type = content_type.substr(0, pos);
        }
        if(content_length_str.length() != 0)
            content_length = stoi(content_length_str);
        if(request.length() != 0){
            if((pos = request.find(" HTTP")) != std::string::npos)
                request = request.substr(0, pos);
            if((pos = request.find("?")) != std::string::npos){
                content = request.substr(pos + 1, request.length());
                request =  request.substr(0, pos);
                content_type = "application/x-www-form-urlencoded";
                content_length = content.length();
            }
        }
        else{
            type = HTTP_TYPE::RESPONSE;
        }
    }
    void parseContent(){
        if(content.length() == 0)
            return;
        if(content_type == "application/json"){
            std::string err;
            jsonContent = json11::Json::parse(content, err);
        }
        else if(content_type == "application/x-www-form-urlencoded"){
            std::map<std::string, json11::Json> moop;
            std::string s = content;
            int pos = 0;
            std::string delimiter = "&";
            while ((pos = s.find(delimiter)) != std::string::npos) {
                std::string token = s.substr(0, pos);
                s.erase(0, pos + delimiter.length());
                if((pos = token.find("=")) != std::string::npos){
                    moop.insert({token.substr(0, pos), json11::Json(token.substr(pos + 1, token.length()))});
                }
            }
            jsonContent = json11::Json(moop);
        }
    }
    HTTPResponse(){
        httpLookups.insert({
            {"http/1.1", &status_msg},
            {"date:", &date},
            {"content-type:", &content_type},
            {"content-length:", &content_length_str},
            {"get", &request},
            {"post", &request},
            {"sec-websocket-accept", &sec_websocket_accept}
        });
    };
};

class HTTP {
    public:
        static HTTPResponse* parseHTTP(std::string buf, int dataLength);
        static HTTPResponse* ssl_recv(SSL* ssl, SOCKET Socket);
        static HTTPResponse* http_recv(SOCKET Socket);
        static HTTPResponse* ssl_GET(SSL* ssl, SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static HTTPResponse* http_GET(SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static HTTPResponse* ssl_POST(SSL* ssl, SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static HTTPResponse* http_POST(SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
        static void http_response(SOCKET Socket, int status_code, std::string status_msg, std::string* headers, int numHeaders, std::string body);
};
#endif