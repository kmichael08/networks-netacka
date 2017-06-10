CPPFLAGS=-Wall -O2 -std=c++14

default: siktacka-client siktacka-server

.PHONY:
	all clean default

siktacka-client.o: utils.h utils.cpp crc32.h crc32.cpp event.h event.cpp client_utils.h client_utils.cpp datagramServerToClient.h datagramServerToClient.cpp client.cpp clock.h clock.cpp game.h game.cpp err.h err.cpp datagramClientToServer.h datagramClientToServer.cpp
	g++ -c $(CPPFLAGS) utils.h utils.cpp crc32.h crc32.cpp event.h event.cpp client_utils.h client_utils.cpp datagramServerToClient.h datagramServerToClient.cpp client.cpp clock.h clock.cpp game.h game.cpp err.h err.cpp datagramClientToServer.h datagramClientToServer.cpp

siktacka-client: siktacka-client.o
	g++ event.o utils.o crc32.o client_utils.o datagramServerToClient.o datagramClientToServer.o client.o clock.o game.o err.o -o siktacka-client

siktacka-server.o: clock.h clock.cpp crc32.h crc32.cpp server.h server.cpp game.h game.cpp event.h event.cpp main.cpp err.h err.cpp datagramClientToServer.h datagramClientToServer.cpp datagramServerToClient.h datagramServerToClient.cpp
	g++ -c $(CPPLAGS) clock.h clock.cpp crc32.h crc32.cpp server.h server.cpp game.h game.cpp event.h event.cpp main.cpp err.h err.cpp datagramClientToServer.h datagramClientToServer.cpp datagramServerToClient.h datagramServerToClient.cpp

siktacka-server: server.o clock.o crc32.o utils.o game.o event.o err.o main.o datagramClientToServer.o datagramServerToClient.o
	g++ clock.o crc32.o server.o utils.o game.o event.o err.o main.o datagramClientToServer.o datagramServerToClient.o -o siktacka-server

clean:
	rm *.o
	rm *.gch
	rm siktacka-client
	rm siktacka-server

all: siktacka-client siktacka-server

