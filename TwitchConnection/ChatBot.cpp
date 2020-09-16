#include "ChatBot.h"
#include <random>


ChatBot::ChatBot(EventQueue* p_eventQueue, followerDict* followers): eventQueue(p_eventQueue){
    Commands::followers = followers;
    connectSocket();
}

void ChatBot::processEvent(updateEvent* event){
    if(event->action == updateEvent::ACTION::NEW_FOLLOWER){
        std::string followMSG = "Thank you for following @" + (*(event->info))["data"][0]["from_name"].string_value();
        followMSG += "\nUse !color to change the color of your cube.";
        sendPrivMsg(followMSG);
    }
}

void ChatBot::runWebSocket(){
    performHandshake();
    IRCAuthenticate();
    unsigned char* buf = new unsigned char[RECV_BUF_SIZE];

    int dataLength = 0;
    while(1){
        if((dataLength = SSL_read(ssl, buf, RECV_BUF_SIZE)) > 0){
            processMsg(buf);
        }
    }
    delete[] buf;
}

void mask(unsigned char* buf, int length, unsigned char* mask){
    for(int i=0;i<length;i++){
        buf[i] ^= mask[i % 4];
    }
}

unsigned char* ChatBot::frame(std::string msg, uint8_t opCode, int &length){
    int dataStart = 6;
    short size = short(msg.length());
    if(size > 125 && msg.length() <= 65535){
        dataStart += 2;
    }
    else if(size > 65535)
        dataStart += 4;

    unsigned char* buf = new unsigned char[dataStart + size];
    memset(buf, 0, dataStart + size);
    buf[0] = uint8_t(opCode | 0x80);
    if(size <= 125){
        buf[1] = uint8_t(size | 0x80);
    }
    else if(size <= 65535){
        buf[1] = uint8_t(126 | 0x80);
        memcpy(&buf[2], &size, sizeof(short));
    }
    else{
        buf[1] = uint8_t(127 | 0x80);
        long long_size = long(msg.length());
        memcpy(&buf[2], &long_size, sizeof(long));
    }
    unsigned char mask_bytes[4];
    mask_bytes[0] = uint8_t(0x37);
    mask_bytes[1] = uint8_t(0xfa);
    mask_bytes[2] = uint8_t(0x21);
    mask_bytes[3] = uint8_t(0x3d);
    memset(mask_bytes, 0, 4);
    memcpy(&buf[dataStart - 4], mask_bytes, sizeof(uint32_t));
    strcpy((char*)&buf[dataStart], msg.c_str());
    mask(&buf[dataStart], size, mask_bytes);
    length = dataStart + size;
    return buf;
}
std::string ChatBot::unFrame(unsigned char* frame){
    
    std::bitset<8> opCode(frame[0]);
    std::bitset<7> payload_len_bits(frame[1]);
    long payload_len = payload_len_bits.to_ulong();
    int dataStart = 2;
    if(payload_len == 126){
        dataStart += 2;
    }
    else if(payload_len == 127){
        dataStart += 4;
    }
    
    if((frame[1] & 0x80) == 0x80){
        dataStart += 4;
        mask(&frame[dataStart], payload_len, &frame[dataStart - 4]);
    }
    std::string data(reinterpret_cast<char*>(&frame[dataStart]), payload_len);
    if((frame[0] & 0x0F) == pongOpCode){
        sendMsg(data, pongOpCode);
    }
    return data;
}

void ChatBot::processMsg(unsigned char* msg){
    std::string command = unFrame(msg);
    if(command.find("PING") != command.npos){
        sendMsg("PONG :tmi.twitch.tv", textOpCode);
        std::cout << "Sent Pong" << std::endl;
    }
    Command commandObj = Command();
    std::string user;
    int pos = 0;
    if((pos = command.find("PRIVMSG")) != command.npos){
        command.erase(command.length() - 2, command.length());
        pos = command.find(":");
        commandObj.user = command.substr(pos + 1, command.find("!") - 1);
        command.erase(0, pos + 1);
        pos = command.find(":");
        command.erase(0, pos + 1);
        if(command.substr(0, commandIdentifier.length()) != commandIdentifier)
            return;
        else
            command.erase(0, commandIdentifier.length());
        if((pos = command.find(" ")) != command.npos){
            commandObj.command = command.substr(0, pos);
            commandObj.data = command.substr(pos + 1, command.length());
        }
        else{
            commandObj.command = command;
        }
        executeCommand(commandObj);
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
    sendMsg("PRIVMSG #goopy131 :" + msg, textOpCode);
}

void ChatBot::sendMsg(std::string msg, uint8_t opCode){
    if(!conEstablished){
        std::cout << "ChatBot connection not established" << std::endl;
        return;
    }
    int length = 0;
    unsigned char* frame_bytes = frame(msg, opCode, length);
    SSL_write(ssl, frame_bytes, length);
}

void ChatBot::IRCAuthenticate(){
    std::string botOAuth;
    std::ifstream file_handle;
    file_handle.open("dontShowOnStream.txt");
    file_handle >> botOAuth;
    file_handle >> botOAuth;
    file_handle >> botOAuth;
    file_handle.close();
    sendMsg("PASS oauth:" + botOAuth, textOpCode);
    sendMsg("NICK goopybot3000", textOpCode);
    sendMsg("JOIN #goopy131", textOpCode);
}

void ChatBot::performHandshake(){
    std::string* headers = new std::string[4];
    headers[0] = "Upgrade: websocket";
    headers[1] = "Connection: upgrade";
    headers[2] = "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==";
    headers[3] = "Sec-WebSocket-Version: 13";
    HTTPResponse* response = HTTP::ssl_GET(ssl, webSocket, CHATURL, "", headers, 4, nullptr, 0);
    if(response->status_code == HTTPResponse::STATUS_CODES::SWITCHING_PROTOCOLS){
        std::cout << "ChatBot websocket established" << std::endl;
        conEstablished = true;
    }
    else{
        printf("Failed to establish ChatBot\n%d %s\nmessage: %s\n", response->status_code, response->status_msg.c_str(), response->jsonContent["message"].string_value().c_str());
    }
}

void ChatBot::connectSocket(){
    int result;
    webSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    result = getaddrinfo(CHATURL.c_str(), SSL_PORT, &hints, &addrInfo);
    if ( result != 0 ) {
        printf("getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return;
    }

    result = connect(webSocket, addrInfo->ai_addr, addrInfo->ai_addrlen);
    if(result == SOCKET_ERROR){
        printf(("Failed to connect: Error " + std::to_string(WSAGetLastError()) + "\n").c_str());
        closesocket(webSocket);
        webSocket = INVALID_SOCKET;
        return;
    }
    SSL_load_error_strings();
    SSL_library_init();
    ctx = SSL_CTX_new(SSLv23_client_method());
    ssl = SSL_new(ctx);
    if(!ssl) {
        closesocket(webSocket);
        fprintf(stderr, "SSL creation error\n");
    }
    SSL_set_fd(ssl, webSocket);
    result = SSL_connect(ssl);
    if(!result) {
        closesocket(webSocket);
        fprintf(stderr, "SSL connect error\nretval: %d\n", result);
        result = SSL_get_error(ssl, result);
        fprintf(stderr, "SSL error: %d\n", result);
    }
}