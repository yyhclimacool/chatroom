all: chatroom_server chatroom_client

CC = g++

CFLAGS = --std=c++0x -g

chatroom_server: ServerMain.cpp Server.o
	$(CC) $(CFLAGS) -o chatroom_server ServerMain.cpp Server.o

Server.o: Server.cpp Server.h Common.h
	$(CC) $(CFLAGS) -c Server.cpp 

chatroom_client: ClientMain.cpp Client.o
	$(CC) $(CFLAGS) -o chatroom_client ClientMain.cpp Client.o

Client.o: Client.cpp Client.h Common.h
	$(CC) $(CFLAGS) -c Client.cpp

clean:
	-rm *.o
	-rm chatroom_server chatroom_client
