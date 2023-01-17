#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/Event.h"
#include <string>
#include <vector>
#include <map>
using namespace std;


class StompProtocol
{
private:
string username;
int subscriptionCounter;
int commandsLeft;
map<string, int> topicToSubId;
map <pair<string, string> , vector<Event>*> & pairToEventList;


//keyboardToFrame is to use these methods:
void CONNECT(vector<string>& input);
void SEND(vector<string>& input);
string createSendFrame(string opening, Event& event);
string createSendFrameOpening(names_and_events& names_and_events);
void SUBSCRIBE(vector<string>& input);
void UNSUBSCRIBE(vector<string>& input);
void DISCONNECT();
void SUMMARIZE(vector<string>& input);
string makeStats(vector<string> &input, vector<Event> *eventList, string gameName);
void writeToFile(string toWrite, string fileName);
void sendFrame(string frame);

//serverToReaction is to use the following:
void CONNECTED(vector<string>& input);
void MESSAGE(vector<string>& input);
Event jesusCristWeNeedToCreateEvent(vector<string>& input);
void RECEIPT(vector<string>& input);
void ERROR(string frame);


public:
bool shouldTerminate;
ConnectionHandler* handler;

~StompProtocol();

StompProtocol(map <pair<string, string> , vector<Event>*> & pairToEventList);
void keyboardToFrame(string line);
string serverToReaction(string frame);
ConnectionHandler* getHandler();
StompProtocol(const StompProtocol&);
StompProtocol& operator=(const StompProtocol&);


};
