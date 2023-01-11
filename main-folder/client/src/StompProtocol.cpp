#include <sstream>
#include <vector>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include <limits>
#include <list>
#include <vector>

using namespace std;

StompProtocol::StompProtocol(map <tuple<string, string>, list<Event&>> & tupleToEventList) : username(""), subscriptionCounter(0), handler(nullptr), topicToSubId(), commandsLeft(0), shouldTerminate(false),tupleToEventList(tupleToEventList) {}

StompProtocol::~StompProtocol(){
    if(handler != nullptr){
        delete handler;
        handler = nullptr;
    }
}

void StompProtocol::keyboardToFrame(string line)//**********************************//
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
    }

    if (type == "login"){
        CONNECT(words);
        commandsLeft++;
    }

    else if(handler != nullptr){
        if (type == "join"){
            SUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == "exit"){
            UNSUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == "report") // TODO implement
            SEND(words);
        
        else if (type == "logout"){
            DISCONNECT();
            commandsLeft++;
        }
    

        else
            cout << "bad command" << endl;
    }

    else cout << "Client not yet connected" << endl;

    // TODO: support summary + print error
    if(handler != nullptr && frame != "")
        handler->sendMessage(frame);

    if(shouldDisconnect)
        shouldTerminate = true;

}

string StompProtocol::serverToReaction(string frame){//**********************************//
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

void StompProtocol::CONNECT(vector<string> &input)
{
    if (username != "")
    {
        std::cout << "The client is already logged in, log out before trying again" << endl;
    }

    string frame = "CONNECT\n";
    frame += "accept-version 1.2\n";

    string host = input[1];

    frame = frame + "host:" + input[1] + '\n';
    frame = frame + "login:" + input[3] + '\n';
    frame = frame + "passcode:" + input[4] + '\n' + '\n' + '\0';

    int port = stoi(input[2]);
    handler = new ConnectionHandler(host, port);
    handler->connect();
    sendFrame(frame);
}
// WARNING: code is not yet good for unecessary spaces
void StompProtocol::SEND(vector<string> &input)
{
    string fileName = input[1];
    names_and_events names_and_events = parseEventsFile(fileName);
    string opening = createSendFrameOpening(names_and_events);
    string frame;
    for(Event event : names_and_events.events){
        frame = createSendFrame(opening, event);
        sendFrame(frame);
    } 
}

string StompProtocol::createSendFrame(string opening, Event& event){
    string frame = opening + "event name:" + event.get_name() + "\n" + "time:" + to_string(event.get_time()) + "\n" +
                   "team a updates:" + event.fcku_a() + "team b updates:" + event.fcku_b() + event.get_discription() + "\n\0"; 
    return frame;
}

string StompProtocol::createSendFrameOpening(names_and_events& names_and_events){
    string gameName = names_and_events.team_a_name + "-" + names_and_events.team_b_name;
    string opening = "SEND\n";
    opening += "destination:/" + gameName + "\n\n" + "user:" + username + "\n"
    + "team a:" + names_and_events.team_a_name +  "team b:" + names_and_events.team_b_name + "\n";
    return opening;
}

void StompProtocol::SUBSCRIBE(vector<string> &input)
{

    string frame = "SUBSCRIBE" + '\n';
    frame += "destination:/" + input[1] + '\n';
    frame += "id:" + to_string(subscriptionCounter) + '\n';
    frame += "receipt:" + to_string(subscriptionCounter + 5) + '\n' + '\n' + '\0';

    topicToSubId[input[1]] = subscriptionCounter;

    subscriptionCounter++;
    sendFrame(frame);
}

void StompProtocol::UNSUBSCRIBE(vector<string> &input)
{
    string frame = "UNSUBSCRIBE" + '\n';
    int id = topicToSubId.at(input[1]);
    frame += "id:" + to_string(id) + '\n';
    frame += "receipt:" + to_string(id + 5) + '\n' + '\n' + '\0';

    sendFrame(frame);
}

void StompProtocol::DISCONNECT()
{
    string frame = "DISCONNECT" + '\n';
    frame += "receipt:" + to_string(-1) + '\n' + '\n' + '\0';
    sendFrame(frame);
}

void StompProtocol::sendFrame(string frame)
{
    if(handler != nullptr && frame != "")
        handler->sendMessage(frame);
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
