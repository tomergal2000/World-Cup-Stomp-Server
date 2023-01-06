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
unordered_map<string, int> topicToSubId;


//keyboardToFrame is to use these methods:
string CONNECT(vector<string>& input);
string SEND(vector<string>& input);
string SUBSCRIBE(vector<string>& input);
string UNSUBSCRIBE(vector<string>& input);
string DISCONNECT();

//serverToReaction is to use the following:
string CONNECTED(vector<string>& input);
string MESSAGE(vector<string>& input);
string RECEIPT(vector<string>& input);
string ERROR(vector<string>& input);

public:
ConnectionHandler handler;

StompProtocol();
string keyboardToFrame(string line);
string serverToReaction(string frame);


};
