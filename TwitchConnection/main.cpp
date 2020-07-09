#include <winsock2.h>
#include <string>
#include "TwitchConnection.h"
#include "NotifyServer.h"
#include <thread>

std::string clientID = "akzng2tgeto835pkwelxcu8x53bu85";
std::string clientSecret = "1j4omdc704xbr53t5bvya7dmlve706";

void startTwitchServer(){
    NotifyServer* server = new NotifyServer();
}

int main(int argc, char* argv[]){
    std::string twitchUrl = "id.twitch.tv";
    TwitchConnection* con = new TwitchConnection();
    con->establishConnection(twitchUrl);
    con->getOauthToken();
    con->closeConnection();
    std::thread server (serverThread);
    con->establishConnection("api.twitch.tv");
    con->subscribeToFollower();
    server.join();
}   