#include "../include/KeyboardToServer.h"
#include "../include/StompProtocol.h"

using namespace std;

KeyboardToServer::KeyboardToServer(StompProtocol& protocol):protocol(&protocol){}

void KeyboardToServer::run() {
    while(!(protocol->shouldTerminate)) {
        const short bufsize = 1024;
        char buf[bufsize];
        cin.getline(buf, bufsize);
        string line(buf);
        if(line.length() > 1)
            protocol->keyboardToFrame(line);
        sleep(1);
    }
}

KeyboardToServer::KeyboardToServer(const KeyboardToServer &kts): protocol(kts.protocol) {}

KeyboardToServer& KeyboardToServer::operator=(const KeyboardToServer &kts){
    protocol = kts.protocol;
    return *this;
}

KeyboardToServer::~KeyboardToServer() {
    delete protocol;
}


