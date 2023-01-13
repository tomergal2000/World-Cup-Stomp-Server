#include "../include/StompProtocol.h"

using namespace std;

class KeyboardToServer{
    private:
    StompProtocol* protocol;

    public:
    KeyboardToServer(StompProtocol& protocol);
    void run();
    KeyboardToServer(const KeyboardToServer& kts);
    KeyboardToServer& operator=(const KeyboardToServer &kts);
    ~KeyboardToServer();



};