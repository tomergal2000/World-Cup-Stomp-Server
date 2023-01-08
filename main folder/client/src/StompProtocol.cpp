#include <sstream>
#include <vector>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include <limits>

using namespace std;

StompProtocol::StompProtocol() : username(""), subscriptionCounter(0), handler(nullptr), topicToSubId(), commandsLeft(0), shouldTerminate(false) {}

StompProtocol::~StompProtocol(){
    if(handler != nullptr){
        delete handler;
        handler = nullptr;
    }
}

string StompProtocol::keyboardToFrame(string line)
{
    string type;
    string frame = "";
    vector<string> words;
    bool shouldDisconnect = false;

    stringstream ss(line);
    string word;
    while (ss >> word)
        words.push_back(word);

    //for debugging:
    for(string w : words) cout << w << endl;
    
    type = words[0];

    if(username == "" && type != "login"){
        cout << "The client is not yet logged in" << endl;
        return "";
    }

    if (type == "login"){
        frame = CONNECT(words);
        commandsLeft++;
    }

    else if(handler != nullptr){
        if (type == "join"){
            frame = SUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == "exit"){
            frame = UNSUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == "report") // TODO implement
            frame = SEND(words);
        else if (type == "logout"){
            frame = DISCONNECT();
            commandsLeft++;
        }

        else
            cout << "bad command" << endl;
    }

    else cout << "Client not yet connected" << endl;

    // TODO: support summarry + print error
    if(handler != nullptr && frame != "")
        handler->sendMessage(frame);

    if(shouldDisconnect)
        shouldTerminate = true;

}

string StompProtocol::serverToReaction(string frame){
    string type;
    vector<string> words;

    stringstream ss(frame);
    string word;
    while (ss >> word)
        words.push_back(word);

    type = words[0];

    //for debugging:
    for(string w : words) cout << w << endl;

    if (type == "CONNECTED"){
        cout << "Login successful" << endl;
        commandsLeft--;
    }
    else if (type == "MESSAGE")
        MESSAGE(words);

    else if (type == "RECEIPT"){
        RECEIPT(words);
        commandsLeft--;
    }

    else if (type == "ERROR"){
        commandsLeft = 0;
        ERROR(words);
    }

    else
        std::cout << "bad frame from server" << endl;

    // TODO: return type?

    return frame;
}

ConnectionHandler* StompProtocol::getHandler()
{
    return handler;
}

// output functions:

string StompProtocol::CONNECT(vector<string> &input)
{
    if (username != "")
    {
        std::cout << "The client is already logged in, log out before trying again" << endl;
        return "";
    }

    string frame = "CONNECT\n";
    frame += "accept-version 1.2\n";

    string host;

    frame = frame + "host:" + input[1] + '\n';

    frame = frame + "login:" + input[3] + '\n';
    frame = frame + "passcode:" + input[4] + '\n' + '\n' + '\0';

    int port = stoi(input[2]);
    handler = new ConnectionHandler(host, port);
    handler->connect();
    return frame;
}

string StompProtocol::SEND(vector<string> &input)
{
    return "";
}

string StompProtocol::SUBSCRIBE(vector<string> &input)
{

    string frame = "SUBSCRIBE" + '\n';
    frame += "destination:/" + input[1] + '\n';
    frame += "id:" + to_string(subscriptionCounter) + '\n';
    frame += "receipt:" + to_string(subscriptionCounter + 5) + '\n' + '\n' + '\0';

    topicToSubId[input[1]] = subscriptionCounter;

    subscriptionCounter++;
    return frame;
}

string StompProtocol::UNSUBSCRIBE(vector<string> &input)
{
    string frame = "UNSUBSCRIBE" + '\n';
    int id = topicToSubId.at(input[1]);
    frame += "id:" + to_string(id) + '\n';
    frame += "receipt:" + to_string(id + 5) + '\n' + '\n' + '\0';

    return frame;
}

string StompProtocol::DISCONNECT()
{
    string frame = "DISCONNECT" + '\n';
    frame += "receipt:" + to_string(-1) + '\n' + '\n' + '\0';
    return frame;
}

// input functions:

void StompProtocol::MESSAGE(vector<string>& words)
{
    cout << "Recieved message from topic: " + words[1].substr(13) << endl;
    cout << "Message content: " + words[4]<< endl;
}

void StompProtocol::RECEIPT(vector<string>& words)
{
    int receipt = stoi(words[1].substr(11));
    if (receipt == -1){
        delete handler;
        while(commandsLeft != 0){
            cout << "busy-waiting" << endl; //hopefully this never happens
        }
        shouldTerminate = true;
    }
    else{
        //make sure receipt is relevant
    }
}

void StompProtocol::ERROR(vector<string>& words)
{
    for(int i = 3; i < words.size(); i++){
        cout << words[i] << endl;
    }
    delete handler;
    shouldTerminate = true;
}
