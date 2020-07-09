#ifndef CHATBOT_H
#define CHATBOT_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <bitset>
#include "HTTP.h"
#include "TwitchStructs.hpp"
#include "Commands.h"

#define RECV_BUF_SIZE 8192
#define SSL_PORT "443"



class ChatBot {
public:
    ChatBot(updateEvent* update, followerDict* followers);
    void sendMsg(std::string msg, uint8_t opCode);
    void sendPrivMsg(std::string msg);
    void processMsg(unsigned char* msg);
private:
    SOCKET webSocket = INVALID_SOCKET;
    struct addrinfo* addrInfo = NULL;
    updateEvent* toUpdate = nullptr;
    const std::string CHATURL = "irc-ws.chat.twitch.tv";
    SSL_CTX *ctx;
    SSL *ssl;
    void connectSocket();
    void runWebSocket();
    void performHandshake();
    void IRCAuthenticate();
    bool conEstablished;
    const std::string commandIdentifier = "!";
    std::string oAuthToken;
    uint8_t textOpCode = 0x01;
    uint8_t closeOpCode = 0x08;
    uint8_t pongOpCode = 0x0A;
    unsigned char* frame(std::string msg, uint8_t opCode, int &length);
    std::string unFrame(unsigned char* frame);
    void executeCommand(Command com);

};
#endif