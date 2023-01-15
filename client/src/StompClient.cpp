#include "../include/StompProtocol.h"
#include <thread>
#include <map>
#include <list>
#include "../include/Event.h"
#include "../include/KeyboardToServer.h"

using namespace std;

int main(int argc, char *argv[])
{

	map<pair<string, string>, vector<Event>*> pairToEventList = map<pair<string, string>, vector<Event>*>();

	while (1)
	{

		StompProtocol *protocol = new StompProtocol(pairToEventList);

		KeyboardToServer kt = KeyboardToServer(*protocol);
		thread KEYTHREAD(&KeyboardToServer::run, &kt);

		// This is the main thread. It reads from the server and reacts.

		ConnectionHandler *handler = protocol->getHandler(); // this might be not good bc it's null in the beginning...

		while (!protocol->shouldTerminate)
		{
			string ans;
			if (handler != nullptr)
			{
				cout << "waiting for message" << endl;
				if (handler->getMessage(ans))
				{
					protocol->serverToReaction(ans);
				}
			}
			else{
				handler = protocol->getHandler();
			}
		}

		KEYTHREAD.join();
		// delete (protocol);
		cout << "Disconnected. Exiting...\n" << std::endl;
	}
	//delete the eventLists
	for(pair<pair<string, string>, vector<Event>*> keyValue : pairToEventList){
		delete &keyValue.second;
	}
	return 0;
}