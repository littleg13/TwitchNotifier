#include "ChatBot.h"
#include <random>


ChatBot::ChatBot(EventQueue* p_eventQueue, UserDict* users): eventQueue(p_eventQueue){
    Commands::users = users;
    Commands::eventQueue = eventQueue;
    webSocket = new WebSocket(CHATURL);
}

void ChatBot::processEvent(updateEvent* event){
    if(event->action == updateEvent::ACTION::NEW_FOLLOWER){
        std::string followMSG = "Thank you for following @" + event->user->name;
        followMSG += "\nUse !color to change the color of your cube.";
        sendPrivMsg(followMSG);
    }
}

void ChatBot::runWebSocket(){
    IRCAuthenticate();
    webSocket->listen(this, processMsg);
}

void ChatBot::processMsg(void* instance, std::string msg){
    ChatBot* chatBot = reinterpret_cast<ChatBot*>(instance);
    if(msg.find("PING") != msg.npos){
        chatBot->webSocket->send("PONG :tmi.twitch.tv", OPCODE::TEXT);
        std::cout << "Sent Pong" << std::endl;
    }
    Command commandObj = Command();
    std::string user;
    int pos = 0;
    if((pos = msg.find("PRIVMSG")) != msg.npos){
        msg.erase(msg.length() - 2, msg.length());
        pos = msg.find(":");
        std::string userName = msg.substr(pos + 1, msg.find("!") - 1);
        commandObj.userID = getUserID(userName, Commands::users);
        msg.erase(0, pos + 1);
        pos = msg.find(":");
        msg.erase(0, pos + 1);
        if(msg.substr(0, commandIdentifier.length()) != commandIdentifier)
            return;
        else
            msg.erase(0, commandIdentifier.length());
        if((pos = msg.find(" ")) != msg.npos){
            commandObj.command = msg.substr(0, pos);
            commandObj.data = msg.substr(pos + 1, msg.length());
        }
        else{
            commandObj.command = msg;
        }
        chatBot->executeCommand(commandObj);
    }
}

void ChatBot::executeCommand(Command com){
    if(Commands::Map.find(com.command) != Commands::Map.end()){
        std::string err;
        if(Commands::Map[com.command](com, err))
            sendPrivMsg(err);
    }
}

void ChatBot::sendPrivMsg(std::string msg){
    webSocket->send("PRIVMSG #goopy131 :" + msg, OPCODE::TEXT);
}

void ChatBot::IRCAuthenticate(){
    std::string botOAuth;
    std::ifstream file_handle;
    file_handle.open("dontShowOnStream.txt");
    file_handle >> botOAuth;
    file_handle >> botOAuth;
    file_handle >> botOAuth;
    file_handle.close();
    webSocket->send("PASS oauth:" + botOAuth, OPCODE::TEXT);
    webSocket->send("NICK goopybot3000", OPCODE::TEXT);
    webSocket->send("JOIN #goopy131", OPCODE::TEXT);
}
