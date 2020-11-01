#ifndef CHATBOT_H
#define CHATBOT_H


#include <fstream>
#include <stdio.h>
#include <string>
#include "WebSocket.h"
#include "user.hpp"
#include "Commands.h"
#include "EventQueue.h"

#define commandIdentifier std::string("!")




class ChatBot {
public:
    ChatBot(EventQueue* p_eventQueue, UserDict* users);
    void sendPrivMsg(std::string msg);
    static void processMsg(void* instance, std::string msg);
    void processEvent(updateEvent* update);
    void runWebSocket();
private:
    WebSocket* webSocket;
    EventQueue* eventQueue = nullptr;
    const std::string CHATURL = "irc-ws.chat.twitch.tv";
    void executeCommand(Command com);
    void IRCAuthenticate();
    std::string oAuthToken;
    

};
#endif