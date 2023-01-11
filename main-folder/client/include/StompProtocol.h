#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/Event.h"
#include <string>
#include <vector>
#include <list>
#include <map>
using namespace std;


class StompProtocol
{
private:
map<string, map<string, vector<string>>> userMaps;
string username;
int subscriptionCounter;
int commandsLeft;
map<string, int> topicToSubId;
map <tuple<string, string> , list<Event&>> & tupleToEventList;


//keyboardToFrame is to use these methods:
void CONNECT(vector<string>& input);
void SEND(vector<string>& input);
string StompProtocol::createSendFrame(string opening, Event& event);
string StompProtocol::createSendFrameOpening(names_and_events& names_and_events);
void SUBSCRIBE(vector<string>& input);
void UNSUBSCRIBE(vector<string>& input);
void DISCONNECT();

//serverToReaction is to use the following:
void CONNECTED(vector<string>& input);
void MESSAGE(vector<string>& input);
void RECEIPT(vector<string>& input);
void ERROR(vector<string>& input);
void sendFrame(string frame);

public:
bool shouldTerminate;
~StompProtocol();

ConnectionHandler* handler;

StompProtocol(map <tuple<string, string> , list<Event&>> & tupleToEventList);
void keyboardToFrame(string line);
string serverToReaction(string frame);
ConnectionHandler* getHandler();


};
