#include "NotifyServer.h"

UserDict* NotifyServer::users = nullptr;

NotifyServer::NotifyServer(EventQueue* p_eventQueue, UserDict* p_users): eventQueue(p_eventQueue){
    users = p_users;
    initSocket();
    registerEndpoints();
}

void NotifyServer::registerEndpoints(){
    Endpoints.insert({
        {std::string("/follower"), FollowerEndpoint},
        {std::string("/subscriber"), SubscriberEndpoint},
        {std::string("/color"), ColorEndpoint}
    });
}
void NotifyServer::run(){
    HTTPResponse* response;
    result = listen(listenSocket, SOMAXCONN);
    if(result == SOCKET_ERROR){
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return;
    }
    while(1){
        clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listenSocket);
            WSACleanup();
            return;
        }
        if((response = HTTP::http_recv(clientSocket)) != nullptr){
            if(Endpoints.find(response->request) == Endpoints.end())
                continue;
            if(response->type == HTTPResponse::HTTP_TYPE::OPTIONS){
                std::string* headers = new std::string[3];
                headers[0] = "Access-Control-Allow-Origin: " + response->headers.at("Origin");
                headers[1] = "Access-Control-Allow_methods: POST, GET, OPTIONS";
                headers[2] = "Access-Control-Allow-Headers: Content-Type, Authorization";
                HTTP::http_response(clientSocket, HTTPResponse::STATUS_CODES::NO_CONTENT, "No Content", headers, 3, "");
                delete[] headers;
            }
            else{
                Endpoints[response->request](this, clientSocket, response);
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

void NotifyServer::addEventToQueue(updateEvent* event){
    eventQueue->mut.lock();
    eventQueue->push(event);
    eventQueue->mut.unlock();
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

void NotifyServer::FollowerEndpoint(NotifyServer* notifyServer, SOCKET& socket, HTTPResponse* response){
    if(response->type == HTTPResponse::HTTP_TYPE::GET){
        std::string* headers = new std::string[2];
        headers[0] = "Content-Length: " + std::to_string(response->jsonContent["hub.challenge"].string_value().length());
        headers[1] = "Content-Type: text/plain";
        HTTP::http_response(socket, HTTPResponse::STATUS_CODES::OK, "OK", headers, 2, response->jsonContent["hub.challenge"].string_value());
        delete[] headers;
    }
    else if(response->type == HTTPResponse::HTTP_TYPE::POST){
        HTTP::http_response(socket, HTTPResponse::STATUS_CODES::OK, "OK", nullptr, 0, "");
        updateEvent* event = new updateEvent();
        event->action = updateEvent::ACTION::NEW_FOLLOWER;
        event->user = new User(response->jsonContent["data"][0]);
        notifyServer->addEventToQueue(event);
    }
}

void NotifyServer::SubscriberEndpoint(NotifyServer* notifyServer, SOCKET& socket, HTTPResponse* response){
    if(response->type == HTTPResponse::HTTP_TYPE::GET){
        std::string* headers = new std::string[2];
        headers[0] = "Content-Length: " + std::to_string(response->jsonContent["hub.challenge"].string_value().length());
        headers[1] = "Content-Type: text/plain";
        HTTP::http_response(socket, HTTPResponse::STATUS_CODES::OK, "OK", headers, 2, response->jsonContent["hub.challenge"].string_value());
        delete[] headers;
    }
    else if(response->type == HTTPResponse::HTTP_TYPE::POST){
        HTTP::http_response(socket, HTTPResponse::STATUS_CODES::OK, "OK", nullptr, 0, "");
        updateEvent* event = new updateEvent();
        if(response->jsonContent["data"][0]["event_data"]["is_gift"].bool_value()){
            event->action = updateEvent::ACTION::GIFTED_SUBSCRIBER;
            event->gifter = new User(stoi(response->jsonContent["data"][0]["event_data"]["gifter_id"].string_value()), response->jsonContent["data"][0]["event_data"]["gifter_name"].string_value());
        }
        else
            event->action = updateEvent::ACTION::NEW_SUBSCRIBER;
        event->user = new User(response->jsonContent["data"][0]["event_data"]);
        notifyServer->addEventToQueue(event);
    }
}

void NotifyServer::ColorEndpoint(NotifyServer* notifyServer, SOCKET& socket, HTTPResponse* response){
    std::string* headers = new std::string[2];
    headers[0] = "Access-Control-Allow-Origin: " + response->headers.at("Origin");
    headers[1] = "Content-Length: 0";
    if(response->type == HTTPResponse::HTTP_TYPE::GET){
        std::cout << response->jsonContent.dump() << std::endl;
        HTTP::http_response(socket, HTTPResponse::STATUS_CODES::OK, "OK", headers, 2, "");
    }
    else if(response->type == HTTPResponse::HTTP_TYPE::POST){
        try{
            int userId = stoi(response->jsonContent["userId"].string_value());
            std::string color = response->jsonContent["color"].string_value();
            User* user = users->at(userId);
            HTTP::http_response(socket, HTTPResponse::STATUS_CODES::OK, "OK", headers, 2, "");
            updateEvent* event = new updateEvent();
            event->action = updateEvent::ACTION::CHANGE_COLOR;
            user->color[0] = stoi(color.substr(1, 2), 0, 16) / 255.0;
            user->color[1] = stoi(color.substr(3, 2), 0, 16) / 255.0;
            user->color[2] = stoi(color.substr(5, 2), 0, 16) / 255.0;
            event->user = new User(*user);
            notifyServer->addEventToQueue(event);
            HTTP::http_response(socket, HTTPResponse::STATUS_CODES::FORBIDDEN, "FORBIDDEN", headers, 2, "");
        }
        catch(std::exception &e){
            HTTP::http_response(socket, HTTPResponse::STATUS_CODES::BAD_REQUEST, "BAD REQUEST", headers, 2, "");
        }
    }
}
