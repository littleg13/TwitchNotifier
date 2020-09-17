#include "NotifyServer.h"


NotifyServer::NotifyServer(EventQueue* p_eventQueue): eventQueue(p_eventQueue){
    initSocket();
    acceptChallenge();
}

void NotifyServer::run(){
    HTTPResponse* response;
    while(1){
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return;
        }
        if((response = HTTP::http_recv(clientSocket)) != nullptr){
            if(response->type == HTTPResponse::HTTP_TYPE::POST){
                HTTP::http_response(clientSocket, HTTPResponse::STATUS_CODES::OK, "OK", nullptr, 0, "");
                updateEvent* event = new updateEvent();
                event->action = updateEvent::ACTION::NEW_FOLLOWER;
                event->info = &response->jsonContent;
                eventQueue->mut.lock();
                eventQueue->push(event);
                eventQueue->mut.unlock();
            }
        }
        result = shutdown(clientSocket, SD_SEND);
        if (result == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
        }
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }
}

void NotifyServer::acceptChallenge(){
    result = listen(listenSocket, SOMAXCONN);
    if(result == SOCKET_ERROR){
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    HTTPResponse* response = HTTP::http_recv(clientSocket);
    std::string* headers = new std::string[2];
    headers[0] = "Content-Length: " + std::to_string(response->jsonContent["hub.challenge"].string_value().length());
    headers[1] = "Content-Type: text/plain";
    HTTP::http_response(clientSocket, HTTPResponse::STATUS_CODES::OK, "OK", headers, 2, response->jsonContent["hub.challenge"].string_value());
    result = shutdown(clientSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
    }
    closesocket(clientSocket);
    clientSocket = INVALID_SOCKET;
}

void NotifyServer::initSocket(){
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    result = getaddrinfo("0.0.0.0", LISTEN_PORT, &hints, &addrInfo);
    if ( result != 0 ) {
        printf("getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return;
    }

    result = bind(listenSocket, addrInfo->ai_addr, (int)addrInfo->ai_addrlen);
    if(result == SOCKET_ERROR){
        printf(("Failed to bind: Error " + std::to_string(WSAGetLastError()) + "\n").c_str());
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
        WSACleanup();
        return;
    }
}