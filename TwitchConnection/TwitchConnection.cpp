#include "TwitchConnection.h"
#include <iostream>
#include <stdlib.h>

TwitchConnection::TwitchConnection(){
    std::ifstream file_handle;
    file_handle.open("dontShowOnStream.txt");
    file_handle >> clientID;
    file_handle >> clientSecret;
    file_handle.close();
    int result;
    result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed with error: %d\n", result);
        return;
    }
}

void TwitchConnection::createSocket(){
    Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket == INVALID_SOCKET) {
        printf("Socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }
}

HTTPResponse* TwitchConnection::post(std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams){
    establishConnection(host);
    if(!conEstablished){
        printf("POST request failed: Please establish connection first\n");
        return NULL;
    }
    HTTPResponse* response = HTTP::ssl_POST(ssl, Socket, baseUrl, path, headers, numHeaders, params, numParams);
    closeConnection();
    return response;
}

HTTPResponse* TwitchConnection::get(std::string host, std::string path, std::string* headers, int numHeaders, std::string* params, int numParams){
    establishConnection(host);
    if(!conEstablished){
        printf("GET request failed: Please establish connection first\n");
        return NULL;
    }
    HTTPResponse* response = HTTP::ssl_GET(ssl, Socket, baseUrl, path, headers, numHeaders, params, numParams);
    closeConnection();
    return response;
}

void TwitchConnection::getOauthToken(){
    
    std::string* params = new std::string[4];
    params[0] = "client_id=" + clientID;
    params[1] = "redirect_uri=http://localhost";
    params[2] = "response_type=code";
    params[3] = "scope=chat:edit+chat:read+channel_subscriptions";
    std::string url = "start \"\" chrome --new-window \"https://" + ID_URL + "/oauth2/authorize" + "?" + params[0];
    for(int i=1;i<4;i++){
        url += "&" + params[i];
    }
    url += "\"";
    system(url.c_str());
    SOCKET clientSocket = acceptIncomingConnection("80");
    HTTPResponse* response = HTTP::http_recv(clientSocket);
    HTTP::http_response(clientSocket, HTTPResponse::STATUS_CODES::OK, "OK", nullptr, 0, "<script>window.close()</script>");
    closesocket(clientSocket);
    delete[] params;
    params = new std::string[5];
    params[0] = "client_id=" + clientID;
    params[1] = "client_secret=" + clientSecret;
    params[2] = "code=" + response->params["code"];
    params[3] = "grant_type=authorization_code";
    params[4] = "redirect_uri=http://localhost";
    response = post(ID_URL, "oauth2/token", nullptr, 0, params, 5);
    if(response->status_code == HTTPResponse::STATUS_CODES::OK){
        OauthToken = response->jsonContent["access_token"].string_value();
        tokenType = response->jsonContent["token_type"].string_value();
        tokenType[0] = toupper(tokenType[0]);
    }
    else{
        printf("Failed to get Oauth token\n%d %s\nmessage: %s\n", response->status_code, response->status_msg.c_str(), response->jsonContent["message"].string_value().c_str());
    }
    delete[] params;
}

std::string TwitchConnection::getAuthToken(){
    return OauthToken;
}

void TwitchConnection::subscribeToFollower(){
    std::string* params = new std::string[4];
    params[0] = "hub.callback=http://twitchapi.goopy.us:3000/follower"; //So not my IP
    params[1] = "hub.mode=subscribe";
    params[2] = "hub.topic=https://api.twitch.tv/helix/users/follows?first=1%26to_id=45893043";
    params[3] = "hub.lease_seconds=864000";

    std::string* headers = new std::string[2];
    headers[0] = "Authorization: " + tokenType + " " + OauthToken;
    headers[1] = "client-id: " + clientID;

    HTTPResponse* response = post(API_URL, "helix/webhooks/hub", headers, 2, params, 4);
    if(response->status_code != HTTPResponse::STATUS_CODES::ACCEPTED)
        printf("%d %s\nmessage: %s\n", response->status_code, response->status_msg.c_str(), response->content.c_str());
    delete response;
    delete[] headers;
    delete[] params;
}

void TwitchConnection::subscribeToSubscriber(){
    std::string* params = new std::string[4];
    params[0] = "hub.callback=http://twitchapi.goopy.us:3000/subscriber"; //So not my IP
    params[1] = "hub.mode=subscribe";
    params[2] = "hub.topic=https://api.twitch.tv/helix/subscriptions/events?%broadcaster_id=45893043%26first=1";
    params[3] = "hub.lease_seconds=864000";

    std::string* headers = new std::string[2];
    headers[0] = "Authorization: " + tokenType + " " + OauthToken;
    headers[1] = "client-id: " + clientID;

    HTTPResponse* response = post(API_URL, "helix/webhooks/hub", headers, 2, params, 4);
    if(response->status_code != HTTPResponse::STATUS_CODES::ACCEPTED)
        printf("%d %s\nmessage: %s\n", response->status_code, response->status_msg.c_str(), response->content.c_str());
    delete response;
    delete[] headers;
    delete[] params;
}

void TwitchConnection::getFollowers(std::vector<User*>* user_arr){
    std::string* headers = new std::string[2];
    headers[0] = "Authorization: " + tokenType + " " + OauthToken;
    headers[1] = "client-id: " + clientID;
    std::string* params = new std::string[3];
    params[0] = "to_id=45893043";
    params[1] = "first=8";
    int prevAmount = user_arr->size();
    HTTPResponse* response = get(API_URL, "helix/users/follows", headers, 2, params, 2);
    const json11::Json::array* jsonArr = &response->jsonContent["data"].array_items();
    for(int i=0;i<jsonArr->size();i++){
        User* user = new User(jsonArr->at(i));
        user_arr->push_back(user);
    }
    int numFollowers = response->jsonContent["total"].number_value();
    while(user_arr->size() - prevAmount < numFollowers){
        params[2] = "after=" + response->jsonContent["pagination"]["cursor"].string_value();
        response = get(API_URL, "helix/users/follows", headers, 2, params, 3);
        jsonArr = &response->jsonContent["data"].array_items();
        for(int i=0;i<jsonArr->size();i++){
            User* user = new User(jsonArr->at(i));
            user_arr->push_back(user);
        }
    }
    delete[] headers;
    delete[] params;
    delete response;
}

void TwitchConnection::getSubscribers(std::vector<User*>* user_arr){
    std::string* headers = new std::string[2];
    headers[0] = "Authorization: " + tokenType + " " + OauthToken;
    headers[1] = "client-id: " + clientID;
    std::string* params = new std::string[3];
    params[0] = "broadcaster_id=45893043";
    params[1] = "first=8";
    int prevAmount = user_arr->size();
    HTTPResponse* response = get(API_URL, "helix/subscriptions", headers, 2, params, 2);
    const json11::Json::array* jsonArr = &response->jsonContent["data"].array_items();
    for(int i=0;i<jsonArr->size();i++){
        User* user = new User(jsonArr->at(i));
        user_arr->push_back(user);
    }
    int numSubs = response->jsonContent["total"].number_value();
    while(user_arr->size() - prevAmount < numSubs){
        params[2] = "after=" + response->jsonContent["pagination"]["cursor"].string_value();
        response = get(API_URL, "helix/subscriptions", headers, 2, params, 3);
        jsonArr = &response->jsonContent["data"].array_items();
        for(int i=0;i<jsonArr->size();i++){
            User* user = new User(jsonArr->at(i));
            user_arr->push_back(user);
        }
    }
    delete[] headers;
    delete[] params;
    delete response;
}

void TwitchConnection::getUsers(std::vector<User*>* user_arr){
    getFollowers(user_arr);
    getSubscribers(user_arr);
}

SOCKET TwitchConnection::acceptIncomingConnection(std::string port){
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int result = getaddrinfo("0.0.0.0", port.c_str(), &hints, &addrInfo);
    if ( result != 0 ) {
        printf("getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return -1;
    }

    result = bind(listenSocket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen);
    if(result == SOCKET_ERROR){
        printf(("Failed to bind: Error " + std::to_string(WSAGetLastError()) + "\n").c_str());
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
        WSACleanup();
        return -1;
    }
    result = listen(listenSocket, SOMAXCONN);
    if(result == SOCKET_ERROR){
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    SOCKET clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return -1;
    }

    return clientSocket;
}

void TwitchConnection::establishConnection(std::string url){
    createSocket();
    baseUrl = url;
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    int result = getaddrinfo(baseUrl.c_str(), "443", &hints, &host);
    if ( result != 0 ) {
        printf("getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return;
    }

    result = connect(Socket, host->ai_addr, host->ai_addrlen);
    if(result == SOCKET_ERROR){
        printf(("Failed to connect: Error " + std::to_string(WSAGetLastError()) + "\n").c_str());
        closesocket(Socket);
        Socket = INVALID_SOCKET;
        return;
    }
    SSL_load_error_strings();
    SSL_library_init();
    ctx = SSL_CTX_new(SSLv23_client_method());
    ssl = SSL_new(ctx);
    if(!ssl) {
        closesocket(Socket);
        fprintf(stderr, "SSL creation error\n");
    }
    SSL_set_fd(ssl, Socket);
    result = SSL_connect(ssl);
    if(!result) {
        closesocket(Socket);
        fprintf(stderr, "SSL connect error\nretval: %d\n", result);
        result = SSL_get_error(ssl, result);
        fprintf(stderr, "SSL error: %d\n", result);
    }
    conEstablished = true;
}

void TwitchConnection::closeConnection(){
    int result = shutdown(Socket, SD_SEND);
    closesocket(Socket);
    Socket = INVALID_SOCKET;
    conEstablished = false;
}

