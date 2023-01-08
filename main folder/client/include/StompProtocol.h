#pragma once

#include "../include/ConnectionHandler.h"
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;


// TODO: implement the STOMP protocol
class StompProtocol
{
private: 
string username;
int subscriptionCounter;
int commandsLeft;
unordered_map<string, int> topicToSubId;


//keyboardToFrame is to use these methods:
string CONNECT(vector<string>& input);
string SEND(vector<string>& input);
string SUBSCRIBE(vector<string>& input);
string UNSUBSCRIBE(vector<string>& input);
string DISCONNECT();

//serverToReaction is to use the following:
void CONNECTED(vector<string>& input);
void MESSAGE(vector<string>& input);
void RECEIPT(vector<string>& input);
void ERROR(vector<string>& input);

public:
bool shouldTerminate;
~StompProtocol();

ConnectionHandler* handler;

StompProtocol();
string keyboardToFrame(string line);
string serverToReaction(string frame);
ConnectionHandler* getHandler();


};
