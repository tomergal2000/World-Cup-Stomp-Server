#include "../include/StompProtocol.h"
#include <thread>

int main(int argc, char *argv[]) {
	
	StompProtocol* protocol = new StompProtocol();
	ConnectionHandler* handler = protocol->getHandler();//check

	thread KEYBOARDTOSERVER([](StompProtocol* protocol){
		while(!protocol->shouldTerminate) {
			const short bufsize = 1024;
			char buf[bufsize];
			cin.getline(buf, bufsize);
			string line(buf);
			protocol->keyboardToFrame(line);//this also sends the frame
		}
	}, protocol);

	//This is the main thread. It reads from the server and reacts.

	while(!protocol->shouldTerminate){
		string ans;
		if(!handler->getMessage(ans)){
			cout << "Disconnected. Exiting...\n" << std::endl;
            break;
		}
		protocol->serverToReaction(ans);
	}

	KEYBOARDTOSERVER.join();

	return 0;
}