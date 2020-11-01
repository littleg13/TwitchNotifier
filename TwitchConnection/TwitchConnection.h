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
#include "user.hpp"
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
    SOCKET acceptIncomingConnection(std::string port);
    void getOauthToken();
    std::string getAuthToken();
    void subscribeToFollower();
    void subscribeToSubscriber();
    void getUsers(std::vector<User*>* user_arr);
    void getFollowers(std::vector<User*>* user_arr);
    void getSubscribers(std::vector<User*>* user_arr);

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
    struct addrinfo* addrInfo = NULL;
    std::string OauthToken;
    std::string tokenType;

    std::string clientID = "";
    std::string clientSecret = "";



protected:
    
};
#endif
