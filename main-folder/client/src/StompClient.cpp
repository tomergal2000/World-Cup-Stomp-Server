#include "../include/StompProtocol.h"
#include <thread>
#include <map>
#include <list>
#include "../include/Event.h"

class event;

using namespace std;

int main(int argc, char *argv[]) {
	map <tuple<string, string>, list<&event>> tupleToEventList();

	while(1){

		StompProtocol* protocol = new StompProtocol(&tupleToEventList);
		ConnectionHandler* handler = protocol->getHandler();

		thread KEYBOARDTOSERVER([](StompProtocol* protocol){
			while(!protocol->shouldTerminate) {//maybe while(1)
				const short bufsize = 1024;
				char buf[bufsize];
				cin.getline(buf, bufsize);
				string line(buf);
				protocol->keyboardToFrame(line);//this also sends the frame
			}
		}, protocol);

		//This is the main thread. It reads from the server and reacts.

		// while(!protocol->shouldTerminate){//maybe while(1)
		// 	string ans;
		// 	if(!handler->getMessage(ans)){
		// 		cout << "Disconnected. Exiting...\n" << std::endl;
		// 		break;
		// 	}
		// 	protocol->serverToReaction(ans);
		// }

		while(!protocol->shouldTerminate){
			string ans;
			if(handler->getMessage(ans))
				protocol->serverToReaction(ans);
		}

		cout << "Disconnected. Exiting...\n" << std::endl;

		KEYBOARDTOSERVER.join();

		return 0;
	}

}