#ifndef NOTIFYSERVER_H
#define NOTIFYSERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include "HTTP.h"
#include "TwitchStructs.hpp"

#define RECV_BUF_SIZE 8192
#define LISTEN_PORT "3000"


class NotifyServer {
public:
    NotifyServer(updateEvent* update);

private:
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    struct addrinfo* addrInfo = NULL;
    updateEvent* toUpdate = nullptr;

};
#endif