#include "../include/StompProtocol.h"
#include <thread>

int main(int argc, char *argv[]) {
	
	while(1){

		StompProtocol* protocol = new StompProtocol();
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

		while(!protocol->shouldTerminate){//maybe while(1)
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

}