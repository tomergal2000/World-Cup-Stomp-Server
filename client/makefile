CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompWCIClient

# EchoClient: bin/ConnectionHandler.o bin/echoClient.o 
# 	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompWCIClient: bin/ConnectionHandler.o bin/StompClient.o bin/Event.o bin/StompProtocol.o bin/KeyboardToServer.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompClient.o bin/Event.o bin/StompProtocol.o bin/KeyboardToServer.o $(LDFLAGS)


bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

# bin/echoClient.o: src/echoClient.cpp
# 	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/Event.o: src/Event.cpp
	g++ $(CFLAGS) -o bin/Event.o src/Event.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/KeyboardToServer.o: src/KeyboardToServer.cpp
	g++ $(CFLAGS) -o bin/KeyboardToServer.o src/KeyboardToServer.cpp

.PHONY: clean
clean:
	rm -f bin/*
	