#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <bitset>
#include "HTTP.h"

#define RECV_BUF_SIZE 8192
#define SSL_PORT "443"

enum OPCODE {
    CONTINUE = 0,
    TEXT = 1,
    BINARY = 2,
    CLOSE = 8,
    PING = 9,
    PONG = 10
};

class WebSocket {
public:
    WebSocket(std::string p_url);
    ~WebSocket();
    void listen(void* instance, void(*recvCallback)(void* instance, std::string data));
    void send(std::string data, OPCODE opCode);
private:
    void connectSSL();
    void upgradeSocket();
    unsigned char* frame(std::string msg, OPCODE opCode, int &length);
    std::string unFrame(unsigned char* frame);
    void mask(unsigned char* buf, int length, unsigned char* mask);
    SOCKET webSocket = INVALID_SOCKET;
    std::string url;
    struct addrinfo* addrInfo = NULL;
    SSL_CTX *ctx;
    SSL *ssl;
    bool conEstablished = false;
};
#endif