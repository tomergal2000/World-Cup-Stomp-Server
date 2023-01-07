#include "../include/StompProtocol.h"
#include <thread>

int main(int argc, char *argv[]) {
	
	StompProtocol* protocol = new StompProtocol();
	ConnectionHandler* handler = protocol->getHandler();

	thread KEYBOARDTOSERVER([](StompProtocol* protocol, ConnectionHandler* handler){
		while(true) {
			const short bufsize = 1024;
			char buf[bufsize];
			cin.getline(buf, bufsize);
			string line(buf);
			string out = protocol->keyboardToFrame(line);
			if(out!="")
				handler->sendMessage(out);
		}
	}, protocol, handler);

	//This is the main thread. It reads from the server and reacts.

	while(handler != nullptr){
		string ans;
		if(!handler->getMessage(ans)){
			cout << "Disconnected. Exiting...\n" << std::endl;
            break;
		}
		protocol->serverToReaction(ans);
		/*what else is there to do?
		if ans is a receipt with -1, then the protocol will delete 
		the handler and it will become nullptr, thus the loop would break
		*/
	}

	KEYBOARDTOSERVER.join();

	return 0;
}