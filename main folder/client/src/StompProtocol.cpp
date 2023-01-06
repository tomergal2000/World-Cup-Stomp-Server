#include <sstream>
#include <vector>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include <limits>

using namespace std;

StompProtocol::StompProtocol(): username(""), subscriptionCounter(0), handler("",-1),subIdToTopic(){}

string StompProtocol::keyboardToFrame(string line){
    string type = "";
    string frame = "";
    vector<std::string> words;

    stringstream ss(line);
    string word;
    while (ss >> word) {
    words.push_back(word);
    }

    type = words[0];

    if (type == "login")
        frame = CONNECT(words);

    else if (type == "join")
        frame = SUBSCRIBE(words);
    
    else if (type == "exit")
        frame = UNSUBSCRIBE(words);

    else if (type == "report") //TODO implement
        frame = SEND(words);

    else if (type == "logout")
        frame = DISCONNECT();
    else 
        std::cout << "bad command" << endl;
    //TODO: support summarry

    return frame;
}

string StompProtocol::serverToReaction(string frame){
    string type = "";
    for(char c : frame){
        if(c == '\n') break;
        type += c;
    }

    if(type == "CONNECTED")
        CONNECTED(frame);

    else if (type == "MESSAGE")
        MESSAGE(frame);
    
    else if (type == "RECEIPT")
        RECEIPT(frame);

    else if (type == "ERROR")
        ERROR(frame);

    else 
        std::cout << "bad frame from server" << endl;

    //TODO: return type?

    return frame;
}

//output functions:

string StompProtocol::CONNECT(vector<string>& input){
    if(username != ""){
        std::cout << "The client is already logged in, log out before trying again" << endl;
        return "";
    }

    string frame = "CONNECT\n";
    frame += "accept-version 1.2\n";
    
    string host;

     
    
    frame = frame + "host:" + input[1] + '\n';

    frame = frame + "login:" + input[3] + '\n';
    frame = frame + "passcode:" + input[4] + '\n'+'\n'+'\0';

    int i = std::stoi(input[2]);
    handler.changeHostPort(host, static_cast<short>(i));
    handler.connect();

    return frame;
}

string StompProtocol::SEND(vector<string>& input){
    return "";
}

string StompProtocol::SUBSCRIBE(vector<string>& input){

    string frame="SUBSCRIBE"+'\n';
    frame+="destination:/"+ input[1]+'\n';
    frame+="id:"+ to_string(subscriptionCounter)+'\n';
    frame+="receipt:"+ to_string(subscriptionCounter+5)+'\n'+'\n'+'\0';

    topicToSubId[input[1]]=subscriptionCounter;

    subscriptionCounter++;
    return frame;
}

string StompProtocol::UNSUBSCRIBE(vector<string>& input){
    string frame="UNSUBSCRIBE"+'\n';
    int id=topicToSubId.at(input[1]);
    frame+="id:"+ to_string(id)+'\n';
    frame+="receipt:"+ to_string(id+5)+'\n'+'\n'+'\0';

    return frame;
}

string StompProtocol::DISCONNECT(){
    string frame="DISCONNECT"+'\n';
    frame+= "receipt:"+ to_string(-1)+'\n'+'\n'+'\0';
    return frame;
}


//input functions:

string StompProtocol::CONNECTED(string frame){
    return "";
}

string StompProtocol::MESSAGE(string frame){
    return "";
}

string StompProtocol::RECEIPT(string frame){
    return "";
}

string StompProtocol::ERROR(string frame){
    return "";
}


