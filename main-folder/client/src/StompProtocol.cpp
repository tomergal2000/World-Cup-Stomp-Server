#include <sstream>
#include <vector>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include <limits>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>


using namespace std;

StompProtocol::StompProtocol(map<pair<string, string>, list<Event &>> &pairToEventList) : username(""), subscriptionCounter(0), handler(nullptr), topicToSubId(), commandsLeft(0), shouldTerminate(false), pairToEventList(pairToEventList) {}

StompProtocol::~StompProtocol()
{
    if (handler != nullptr)
    {
        delete handler;
        handler = nullptr;
    }
}

void StompProtocol::keyboardToFrame(string line) //**********************************//
{
    string type;
    string frame = "";
    vector<string> words;
    bool shouldDisconnect = false;

    stringstream ss(line);
    string word;
    while (ss >> word)
        words.push_back(word);

    // for debugging:
    for (string w : words)
        cout << w << endl;

    type = words[0];

    if (username == "" && type != "login")
    {
        cout << "The client is not yet logged in" << endl;
    }

    if (type == "login")
    {
        CONNECT(words);
        commandsLeft++;
    }

    else if (handler != nullptr)
    {
        if (type == "join")
        {
            SUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == "exit")
        {
            UNSUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == "report") // TODO implement
            SEND(words);

        else if (type == "logout")
        {
            DISCONNECT();
            commandsLeft++;
        }
        else if (type == "summarize")
        {
            commandsLeft++;
            SUMMARIZE(words);
            commandsLeft--;
        }

        else
            cout << "bad command" << endl;
    }

    else
        cout << "Client not yet connected" << endl;

    // TODO: support summary + print error
    if (handler != nullptr && frame != "")
        handler->sendMessage(frame);

    if (shouldDisconnect)
        shouldTerminate = true;
}

string StompProtocol::serverToReaction(string frame)
{ //**********************************//
    string type;
    vector<string> words;

    stringstream ss(frame);
    string word;
    while (ss >> word)
        words.push_back(word);

    type = words[0];

    // for debugging:
    cout << "printing frame i got from server, word by word" << endl;
    for (string w : words)
        cout << w << endl;

    if (type == "CONNECTED")
    {
        cout << "Login successful" << endl;
        commandsLeft--;
    }
    else if (type == "MESSAGE")
        MESSAGE(words);

    else if (type == "RECEIPT")
    {
        RECEIPT(words);
        commandsLeft--;
    }

    else if (type == "ERROR")
    {
        commandsLeft = 0;
        ERROR(words);
    }

    else
        std::cout << "bad frame from server" << endl;

    // TODO: return type?

    return frame;
}

ConnectionHandler *StompProtocol::getHandler()
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
    for (Event event : names_and_events.events)
    {
        frame = createSendFrame(opening, event);
        sendFrame(frame);
    }
}

string StompProtocol::createSendFrame(string opening, Event &event)
{
    string frame = opening + "event name:" + event.get_name() + "\n" + "time:" + to_string(event.get_time()) + "\n" +
                   "team a updates:" + event.fcku_a() + "team b updates:" + event.fcku_b() + event.get_discription() + "\n\0";
    return frame;
}

string StompProtocol::createSendFrameOpening(names_and_events &names_and_events)
{
    string gameName = names_and_events.team_a_name + "_" + names_and_events.team_b_name;
    string opening = "SEND\n";
    opening += "destination:/" + gameName + "\n\n" + "user:" + username + "\n" + "team a:" + names_and_events.team_a_name + "team b:" + names_and_events.team_b_name + "\n";
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
    sleep(0.2); //stop accepting keyboars requests for a moment! need to finish disconnecting...
}
void StompProtocol::SUMMARIZE(vector<string> &input)
{

    string gameName = input[1];
    string userName = input[2];
    string fileName = input[3];
    pair<string, string> key(gameName, userName);

    try
    {
        list<Event &> &eventList = pairToEventList.at(key);
        string toInsert = makeStats(input, eventList, gameName);
        writeToFile(toInsert, fileName);
    }
    catch (out_of_range e)
    {
        cout << "no reports were recieved from this user on that topic" << endl;
    }
}

string StompProtocol::makeStats(vector<string> &input, list<Event &> &eventList, string gameName)
{
    int index = gameName.find('_');
    string team_a = gameName.substr(0, index);
    string team_b = gameName.substr(index + 1);
    string stats = team_a + " vs " + team_b + '\n';

    stats += "Game stats:\n" + team_a + " stats:\n";

    for (Event event : eventList)
    {
        stats += event.fcku_a();
    }

    stats += "\n" + team_b + " stats:\n";
    for (Event event : eventList)
    {
        stats += event.fcku_a();
    }

    stats += "\nGame event reports:\n";
    for (Event event : eventList)
    {
        stats += to_string(event.get_time()) + " - " + event.get_name() + ":\n\n";
        stats += event.get_discription() + "\n\n\n";
    }

    return stats;
}

void StompProtocol::writeToFile(string toWrite, string fileName)
{
    ofstream myFile;
    myFile.open(fileName);
    if (myFile)
    {
        myFile << toWrite;
        myFile.close();
    }
    else
    {
        ofstream newFile(fileName);
        newFile << toWrite;
        newFile.close();
    }
}

void StompProtocol::sendFrame(string frame)
{
    if (handler != nullptr && frame != "")
        handler->sendMessage(frame);
}





// input functions:

//used when we receive a message from the server. pushes event to relevant eventList.
void StompProtocol::MESSAGE(vector<string> &words)
{
    cout << "Recieved message from topic: " + words[1].substr(13) << endl;
    cout << "Message content: " + words[4] << endl;

    string userName = words[5];
    string gameName = words[3];
    int slash = gameName.find('/');
    int endOfLine = gameName.find('\n');//maybe we don't need this
    gameName = gameName.substr(slash, endOfLine);

    pair<string, string> key(gameName, userName);
    Event recieved = jesusCristWeNeedToCreateEvent(words);//incomplete function. jesus.
    list<Event&> eventList;
    bool listExists = (pairToEventList.count(key) != 0);
    if(listExists){
        eventList = pairToEventList.at(key);
        eventList.push_back(recieved);
    }
    else{
        eventList = list<Event&>();
        eventList.push_back(recieved);
        pairToEventList[key] = eventList;
    }
    eventList.push_back(recieved);
}

Event StompProtocol::jesusCristWeNeedToCreateEvent(vector<string> &words)
{
    string user = words[5];
    string team_a = words[7];
    string team_b = words[9];
    string eventName = words[11];
    int time = stoi(words[13]);
    

    map<string, string> fictionalGameUpdatesToSatisfyConstructor = map<string, string>();

    //creation of team a map:
    map<string, string> team_a_map = map<string, string>();
    int wordsIndex = 15;
    while(words[wordsIndex] != "team b updates"){
        string key = words[wordsIndex];
        string value = words[wordsIndex + 1];
        team_a_map[key] = value;
        wordsIndex += 2;
    }

    //creation of team b map:
    map<string, string> team_b_map = map<string, string>();
    wordsIndex++;
    while(words[wordsIndex] != "description"){
        string key = words[wordsIndex];
        string value = words[wordsIndex + 1];
        team_b_map[key] = value;
        wordsIndex += 2;
    }

    wordsIndex++;
    string description;
    while(wordsIndex < words.size()){
        description += words[wordsIndex] + " ";
        wordsIndex++;
    }

    return Event(team_a, team_b, eventName, time,
                 fictionalGameUpdatesToSatisfyConstructor, 
                 team_a_map, team_b_map, description);
}

void StompProtocol::RECEIPT(vector<string> &words)
{
    int receipt = stoi(words[1].substr(11));
    if (receipt == -1)
    {
        delete handler;
        while (commandsLeft != 0)
        {
            cout << "busy-waiting" << endl; // hopefully this never happens
        }
        shouldTerminate = true;
    }
    else
    {
        // make sure receipt is relevant?
    }
}

//prints the frame we got from the server and disconnects.
void StompProtocol::ERROR(vector<string> &words)
{
    for (int i = 3; i < words.size(); i++)
    {
        cout << words[i] << endl;
    }
    delete handler;
    shouldTerminate = true;
}
