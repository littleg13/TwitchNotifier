#ifndef NOTIFYSERVER_H
#define NOTIFYSERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "HTTP.h"
#include "user.hpp"
#include "EventQueue.h"

#define RECV_BUF_SIZE 8192
#define LISTEN_PORT "3000"

class NotifyServer {
public:
    NotifyServer(EventQueue* p_eventQueue, UserDict* p_users);
    void addEventToQueue(updateEvent* event);
    void run();

private:
    void initSocket();
    void registerEndpoints();
    void acceptChallenge();
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    struct addrinfo* addrInfo = NULL;
    EventQueue* eventQueue = nullptr;
    int result;
    std::unordered_map<std::string, void(*)(NotifyServer* notifyServer, SOCKET& socket, HTTPResponse* response)> Endpoints;
    static UserDict* users;
    static void FollowerEndpoint(NotifyServer* notifyServer, SOCKET& socket, HTTPResponse* response);
    static void SubscriberEndpoint(NotifyServer* notifyServer, SOCKET& socket, HTTPResponse* response);
    static void ColorEndpoint(NotifyServer* notifyServer, SOCKET& socket, HTTPResponse* response);
};
#endif