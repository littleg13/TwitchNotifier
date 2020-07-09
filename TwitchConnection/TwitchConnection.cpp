#include "TwitchConnection.h"
#include <iostream>

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
    params[1] = "client_secret=" + clientSecret;
    params[2] = "grant_type=client_credentials";
    params[3] = "scope=chat:edit+chat:read";
    HTTPResponse* response = post(ID_URL, "oauth2/token", nullptr, 0, params, 4);
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
    params[0] = "hub.callback=http://66.45.159.19:3000"; //So not my IP
    params[1] = "hub.mode=subscribe";
    params[2] = "hub.topic=https://api.twitch.tv/helix/users/follows?first=1%26to_id=45893043";
    params[3] = "hub.lease_seconds=864000";

    std::string* headers = new std::string[2];
    headers[0] = "Authorization: " + tokenType + " " + OauthToken;
    headers[1] = "client-id: " + clientID;

    HTTPResponse* response = post(API_URL, "helix/webhooks/hub", headers, 2, params, 4);
    if(response->status_code != HTTPResponse::STATUS_CODES::ACCEPTED)
        printf("%d %s\nmessage: %s\n", response->status_code, response->status_msg.c_str(), response->content.c_str());
    fflush(stdout);
    delete[] headers;
    delete[] params;
}

std::vector<json11::Json> TwitchConnection::getFollowers(){
    std::string* headers = new std::string[2];
    headers[0] = "Authorization: " + tokenType + " " + OauthToken;
    headers[1] = "client-id: " + clientID;
    std::string* params = new std::string[3];
    params[0] = "to_id=45893043"; //So not my IP
    params[1] = "first=8";
    HTTPResponse* response = get(API_URL, "helix/users/follows", headers, 2, params, 2);
    std::vector<json11::Json> follower_arr = response->jsonContent["data"].array_items();
    int numFollowers = response->jsonContent["total"].number_value();
    while(follower_arr.size() < numFollowers){
        params[2] = "after=" + response->jsonContent["pagination"]["cursor"].string_value();
        response = get(API_URL, "helix/users/follows", headers, 2, params, 3);
        follower_arr.insert(follower_arr.end(), response->jsonContent["data"].array_items().begin(), response->jsonContent["data"].array_items().end());
    }
    return follower_arr;

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

