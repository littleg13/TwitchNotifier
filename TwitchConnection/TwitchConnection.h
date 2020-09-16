#ifndef TWITCHCONNECTION_H
#define TWITCHCONNECTION_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <synchapi.h>
#include <string>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "HTTP.h"
#include "Follower.hpp"
#include <vector>

#include <fstream>

class TwitchConnection {
public:
    TwitchConnection();
    void establishConnection(std::string url);
    void createSocket();
    void closeConnection();
    HTTPResponse* post(std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
    HTTPResponse* get(std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams);
    
    void getOauthToken();
    std::string getAuthToken();
    void subscribeToFollower();
    std::vector<json11::Json> getFollowers();

private:
    WSADATA wsaData;
    const std::string API_URL = "api.twitch.tv";
    const std::string ID_URL = "id.twitch.tv";
    SOCKET Socket = INVALID_SOCKET;
    struct addrinfo* host = NULL;
    std::string baseUrl;
    bool conEstablished = false;
    SSL_CTX *ctx;
    SSL *ssl;

    std::string OauthToken;
    std::string tokenType;

    std::string clientID = "";
    std::string clientSecret = "";



protected:
    
};
#endif
