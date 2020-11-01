#include "HTTP.h"

std::unordered_map<std::string, HTTPResponse::HTTP_TYPE> HTTPResponse::httpRequests = {
    {"GET", HTTP_TYPE::GET},
    {"POST", HTTP_TYPE::POST},
    {"OPTIONS", HTTP_TYPE::OPTIONS}
};

HTTPResponse* HTTP::ssl_recv(SSL* ssl, SOCKET Socket){
    char* buf = new char[RECV_BUF_SIZE];
    std::string data = "";
    int dataLength = 0;
    int count = 0;
    bool readFirst = false;
    u_long mode = 1;
    ioctlsocket(Socket, FIONBIO, &mode);
    while(1){
        if((dataLength = SSL_read(ssl, buf, RECV_BUF_SIZE)) > 0){
            data += std::string(buf, dataLength);
            readFirst = true;
        }
        else{
            if(readFirst && count > 2)
                break;
            if(count > 100){
                printf("Timeout\n");
                break;
            }
            count++;
            Sleep(100);
            int err = SSL_get_error(ssl, dataLength);
        }
    }
    mode = 0;
    ioctlsocket(Socket, FIONBIO, &mode);
    return new HTTPResponse(data, dataLength);
}

HTTPResponse* HTTP::http_recv(SOCKET Socket){
    char* buf = new char[RECV_BUF_SIZE];
    std::string data = "";
    int dataLength;
    int count = 0;
    bool readFirst = false;
    u_long mode = 1;
    ioctlsocket(Socket, FIONBIO, &mode);
    while(1){
        if((dataLength = recv(Socket, buf, RECV_BUF_SIZE, 0)) > 0){
            data += std::string(buf, dataLength);
            readFirst = true;
        }
        else{
            if(readFirst)
                break;
            if(count > 100){
                printf("Timeout\n");
                return nullptr;
            }
            count++;
            Sleep(100);
        }
    }
    delete[] buf;
    mode = 0;
    ioctlsocket(Socket, FIONBIO, &mode);
    return new HTTPResponse(data, dataLength);
}

    HTTPResponse* HTTP::ssl_GET(SSL* ssl, SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams){
        std::string paramString = "";
        if(numParams > 0){
            paramString = "?" + params[0];
            for(int i=1;i<numParams;i++){
                paramString += "&" + params[i];
            }
        }
        std::string query = "GET /" + path + paramString + " HTTP/1.1\r\n" + "Host: " + host + ":443\r\n";
        for(int i=0;i<numHeaders;i++){
            query += headers[i] + "\r\n";
        }
        query += "\r\n";
        int result = SSL_write(ssl, query.c_str(), strlen(query.c_str()));
        return HTTP::ssl_recv(ssl, Socket);
    }
    HTTPResponse* HTTP::http_GET(SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams){
        std::string paramString = "";
        if(numParams > 0){
            paramString = "?" + params[0];
            for(int i=1;i<numParams;i++){
                paramString += "&" + params[i];
            }
        }
        std::string query = "GET /" + path + paramString + " HTTP/1.1\r\n" + "Host: " + host + "\r\n";
        for(int i=0;i<numHeaders;i++){
            query += headers[i] + "\r\n";
        }
        query += "\r\n";
        int result = send(Socket, query.c_str(), strlen(query.c_str()), 0);
        return HTTP::http_recv(Socket);
    }
    HTTPResponse* HTTP::ssl_POST(SSL* ssl, SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams){
        std::string paramString = "";
        if(numParams > 0){
            paramString = "?" + params[0];
            for(int i=1;i<numParams;i++){
                paramString += "&" + params[i];
            }
        }
        std::string query = "POST /" + path + paramString + " HTTP/1.1\r\n" + "Host: " + host + ":443\r\n";
        for(int i=0;i<numHeaders;i++){
            query += headers[i] + "\r\n";
        }
        query += "\r\n";
        int result = SSL_write(ssl, query.c_str(), strlen(query.c_str()));
        return HTTP::ssl_recv(ssl, Socket);
    }
    HTTPResponse* HTTP::http_POST(SOCKET Socket, std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams){
        std::string paramString = "";
        if(numParams > 0){
            paramString = "?" + params[0];
            for(int i=1;i<numParams;i++){
                paramString += "&" + params[i];
            }
        }
        std::string query = "POST /" + path + paramString + " HTTP/1.1\r\n" + "Host: " + host + "\r\n";
        for(int i=0;i<numHeaders;i++){
            query += headers[i] + "\r\n";
        }
        query += "\r\n";
        int result = send(Socket, query.c_str(), strlen(query.c_str()), 0);
        return HTTP::http_recv(Socket);
    }

void HTTP::http_response(SOCKET Socket, int status_code, std::string status_msg, std::string* headers, int numHeaders, std::string body){
    std::string query = "HTTP/1.1 " + std::to_string(status_code) + " " + status_msg + "\r\n";
    for(int i=0;i<numHeaders;i++){
        query += headers[i] + "\r\n";
    }
    query += "\r\n";
    query += body + "\r\n";
    int result = send(Socket, query.c_str(), strlen(query.c_str()), 0);
}