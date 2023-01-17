#include <sstream>
#include <vector>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include <limits>
#include <vector>
#include <fstream>
#include <iostream>
#include <thread>


using namespace std;

// handler nullptr
StompProtocol::StompProtocol(map<pair<string, string>, vector<Event>*> &pairToEventList) : username(""), subscriptionCounter(0), commandsLeft(0), topicToSubId(),  pairToEventList(pairToEventList), shouldTerminate(false), handler(nullptr){}

StompProtocol::~StompProtocol()
{
    if (handler != nullptr)
    {
        delete handler;
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
    // for (string w : words)
    //     cout << w << endl;

    type = words[0];
    string empty = "";
    string login = "login";
    if (username == empty && type != login)
    {
        cout << "The client is not yet logged in" << endl;
        return;
    }

    if (type == login)
    {
        CONNECT(words);
        commandsLeft++;
    }

    else if (handler != nullptr)
    {
        string join = "join";
        string exit = "exit";
        string report = "report";
        string logout = "logout";
        string summary = "summary";

        if (type == join)
        {
            SUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == exit)
        {
            UNSUBSCRIBE(words);
            commandsLeft++;
        }
        else if (type == report) // TODO implement
            SEND(words);

        else if (type == logout)
        {
            shouldTerminate = true;
            DISCONNECT();
            commandsLeft++;
        }
        else if (type == summary)
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
    if (handler != nullptr && frame != empty)
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
    cout << "printing frame i got from server:\n" + frame << endl;
    
    string connected = "CONNECTED";
    string message = "MESSAGE";
    string receipt = "RECEIPT";
    string error = "ERROR";

    if (type == connected)
    {
        cout << "Login successful" << endl;
        commandsLeft--;
    }
    else if (type == message)
        MESSAGE(words);

    else if (type == receipt)
    {
        commandsLeft--;
        RECEIPT(words);
    }

    else if (type == error)
    {
        commandsLeft = 0;
        ERROR(frame);
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
    string empty = "";
    if (username != empty)
    {
        std::cout << "The client is already logged in, log out before trying again" << endl;
        return;
    }


    string frame = "CONNECT\n";
    frame += "accept-version:1.2\n";

    string hostPort = input[1];
    int indexNekudotaim = hostPort.find(':');
    string host = hostPort.substr(0, indexNekudotaim);
    string portString = hostPort.substr(indexNekudotaim + 1);
    int port = stoi(portString);
    


    frame = frame + "host:" + host + '\n';
    frame = frame + "login:" + input[2] + '\n';
    frame = frame + "passcode:" + input[3] + '\n' + '\n';

    username = input[2];
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
    string teamA_updates = event.fcku_a();
    string teamB_updates = event.fcku_b();
    string frame = opening + "event name: " + event.get_name() + "\n" + "time: " + to_string(event.get_time()) + "\n" +
                   "team a updates:\n" + teamA_updates + "team b updates:\n" + teamB_updates + "description: " + event.get_discription() + "\n";
    return frame;
}

string StompProtocol::createSendFrameOpening(names_and_events &names_and_events)
{
    string gameName = names_and_events.team_a_name + "_" + names_and_events.team_b_name;
    string opening = "SEND\n";
    opening += "destination:/" + gameName + "\n\n" + "user:" + username + "\n" + "team a:" + names_and_events.team_a_name + "\nteam b:" + names_and_events.team_b_name + "\n";
    return opening;
}

//*************************************** end of SEND ********************************************//


void StompProtocol::SUBSCRIBE(vector<string> &input)
{

    string frame = "SUBSCRIBE\n";
    frame += "destination:/" + input[1] + '\n';
    frame += "id:" + to_string(subscriptionCounter) + '\n';
    frame += "receipt:" + to_string(subscriptionCounter + 5) + '\n' + '\n';

    topicToSubId[input[1]] = subscriptionCounter;

    subscriptionCounter++;
    sendFrame(frame);
}

void StompProtocol::UNSUBSCRIBE(vector<string> &input)
{
    string frame = "UNSUBSCRIBE\n";
    int id = topicToSubId.at(input[1]);
    frame += "id:" + to_string(id) + '\n';
    frame += "receipt:" + to_string(id + 5) + "\n\n";

    sendFrame(frame);
}

void StompProtocol::DISCONNECT()
{
    string frame = "DISCONNECT\n";
    frame += "receipt:" + to_string(-1) + "\n\n";
    
    sendFrame(frame);
}


//********************** last output function: summarize + 2 aid functions **********************//
void StompProtocol::SUMMARIZE(vector<string> &input)
{

    string gameName = input[1];
    string userName = input[2];
    string fileName = input[3];
    pair<string, string> key(gameName, userName);

    try
    {
        vector<Event>* eventList = pairToEventList.at(key);
        string toInsert = makeStats(input, eventList, gameName);
        writeToFile(toInsert, fileName);
    }
    catch (out_of_range &e)
    {
        cout << "no reports were recieved from this user on that topic" << endl;
    }
}

string StompProtocol::makeStats(vector<string> &input, vector<Event> *eventList, string gameName)
{
    int index = gameName.find('_');
    string team_a = gameName.substr(0, index);
    string team_b = gameName.substr(index + 1);
    string stats = team_a + " vs " + team_b + '\n';

    stats += "Game stats:\n" + team_a + " stats:\n";

    map<string, string> teamAUpdatesToAdd = map<string, string>();
    map<string, string> teamBUpdatesToAdd = map<string, string>();


    //go over both team's updtaes, copy contents to unique (ToAdd) map, then add them to the string
    for (Event event : *eventList)
    {
        map<string, string> teamAUpdatesInEvent = event.get_team_a_updates();
        //copy contents:
        for(pair<string, string> keyValue : teamAUpdatesInEvent){
            teamAUpdatesToAdd[keyValue.first] = keyValue.second;
        }
    }
    //add to string:
    for(const pair<string, string> keyValue : teamAUpdatesToAdd){
        stats += "    " + keyValue.first + keyValue.second + "\n";
    }

    stats += team_b + " stats:\n";
    for (Event event : *eventList)
    {
        map<string, string> teamBUpdatesInEvent = event.get_team_b_updates();
        //copy contents:
        for(pair<string, string> keyValue : teamBUpdatesInEvent){
            teamBUpdatesToAdd[keyValue.first] = keyValue.second;
        }
    }
    //add to string:
    for(pair<string, string> keyValue : teamBUpdatesToAdd){
        stats += "    " + keyValue.first + keyValue.second + "\n";
    }

    stats += "\nGame event reports:\n";
    for (Event event : *eventList)
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
    string empty = "";
    if (handler != nullptr && frame != empty)
        handler->sendMessage(frame);
}





// input functions:

//used when we receive a message from the server. pushes event to relevant eventList.
void StompProtocol::MESSAGE(vector<string> &words)
{
    string userName = words[4].substr(5);
    string gameName = words[3].substr(13);
    pair<string, string> key(gameName, userName);
    Event recieved = jesusCristWeNeedToCreateEvent(words);
    bool listExists = (pairToEventList.count(key) != 0);
    if(listExists){
        vector<Event>* eventList = pairToEventList.at(key);
        eventList->push_back(recieved);
    }
    else{
        vector<Event>* eventList = new vector<Event>();
        eventList->push_back(recieved);
        pairToEventList[key] = eventList;
    }
}

Event StompProtocol::jesusCristWeNeedToCreateEvent(vector<string> &words)
{
    string user = words[4].substr(5);
    string team_a = words[6].substr(2);
    string team_b = words[8].substr(2);
    string eventName = "";
    long unsigned int index = 11;
    string toStop = "time:";
    while(words[index] != toStop){
        eventName += words[index] + " ";
        index++;
    }
    eventName = eventName.substr(0, -1);
    index++;
    string timeString = words[index];
    int time = stoi(timeString);
    index += 4;
    

    map<string, string> fictionalGameUpdatesToSatisfyConstructor = map<string, string>();

    //creation of team a map:
    map<string, string> team_a_map = map<string, string>();
    toStop = "team";
    while(words[index] != toStop){
        string key = words[index];
        string value = words[index + 1];
        team_a_map[key] = value;
        index += 2;
    }

    //creation of team b map:
    map<string, string> team_b_map = map<string, string>();
    index += 3;
    toStop = "description:";
    while(words[index] != toStop){
        string key = words[index];
        string value = words[index + 1];
        team_b_map[key] = value;
        index += 2;
    }

    index++;
    string description = "";
    while(index < words.size()){
        description += words[index] + " ";
        index++;
    }

    return Event(team_a, team_b, eventName, time,
                 fictionalGameUpdatesToSatisfyConstructor, 
                 team_a_map, team_b_map, description);
}

//****************************** end of the process of recieving a message *******************************//

void StompProtocol::RECEIPT(vector<string> &words)
{
    
    string receiptString = words[1].substr(11);
    int receipt = stoi(receiptString);
    if (receipt == -1)
    {
        while (commandsLeft != 0)
        {
            cout << "busy-waiting" << endl; // hopefully this never happens
        }
        // shouldTerminate = true;
    }
}

//prints the frame we got from the server and disconnects.
void StompProtocol::ERROR(string frame)
{
    cout << frame << endl;
    if(handler){
        delete handler;
        handler = nullptr;
    }
    shouldTerminate = true;
}
