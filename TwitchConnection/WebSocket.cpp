#include "WebSocket.h"

WebSocket::WebSocket(std::string p_url): url(p_url){
    connectSSL();
    upgradeSocket();
}

void WebSocket::listen(void* instance, void(*recvCallback)(void* instance, std::string data)){
    unsigned char* buf = new unsigned char[RECV_BUF_SIZE];
    int dataLength = 0;
    while(1){
        if((dataLength = SSL_read(ssl, buf, RECV_BUF_SIZE)) > 0){
            recvCallback(instance, unFrame(buf));
        }
    }
    delete[] buf;
}

void WebSocket::upgradeSocket(){
    std::string* headers = new std::string[4];
    headers[0] = "Upgrade: websocket";
    headers[1] = "Connection: upgrade";
    headers[2] = "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==";
    headers[3] = "Sec-WebSocket-Version: 13";
    HTTPResponse* response = HTTP::ssl_GET(ssl, webSocket, url, "", headers, 4, nullptr, 0);
    if(response->status_code == HTTPResponse::STATUS_CODES::SWITCHING_PROTOCOLS){
        conEstablished = true;
    }
    else{
        printf("Failed to establish ChatBot\n%d %s\nmessage: %s\n", response->status_code, response->status_msg.c_str(), response->jsonContent["message"].string_value().c_str());
    }
    delete[] headers;
}

void WebSocket::connectSSL(){
    int result;
    webSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    result = getaddrinfo(url.c_str(), SSL_PORT, &hints, &addrInfo);
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

unsigned char* WebSocket::frame(std::string msg, OPCODE opCode, int &length){
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

std::string WebSocket::unFrame(unsigned char* frame){
    OPCODE opCode = OPCODE(frame[0] & 0x0F);
    uint8_t payload_len_bits = frame[1];
    long payload_len = payload_len_bits;
    int dataStart = 2;
    if(payload_len == 126){
        dataStart += 2;
        memcpy(&payload_len, &frame[2], 2);
    }
    else if(payload_len == 127){
        dataStart += 4;
        memcpy(&payload_len, &frame[2], 8);
    }
    
    if((frame[1] & 0x80) == 0x80){
        dataStart += 4;
        mask(&frame[dataStart], payload_len, &frame[dataStart - 4]);
    }
    std::string data(reinterpret_cast<char*>(&frame[dataStart]), payload_len);
    if(opCode == OPCODE::PING){
        std::cout << "Recieved Ping" << std::endl;
        send(data, OPCODE::PONG);
    }
    return data;
}

void WebSocket::send(std::string msg, OPCODE opCode){
    if(!conEstablished){
        std::cout << "Websocket to " << url << " connection not established" << std::endl;
        return;
    }
    int length = 0;
    unsigned char* frame_bytes = frame(msg, opCode, length);
    SSL_write(ssl, frame_bytes, length);;
}

void WebSocket::mask(unsigned char* buf, int length, unsigned char* mask){
    for(int i=0;i<length;i++){
        buf[i] ^= mask[i % 4];
    }
}