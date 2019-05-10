CC=g++
CFLAGS=-g -std=c++11 -pthread
BIN=./bin/
SOURCE=./src/

LIST=$(BIN)server $(BIN)client_udp
CPP = $(SOURCE)socket_helper.cpp

all: $(LIST)

$(BIN)server:  $(SOURCE)server.cpp $(SOURCE)
	$(CC) $< $(CPP) $(CFLAGS) -o $@ $(LIBS)

$(BIN)client_udp:  $(SOURCE)client_udp.cpp
	$(CC) $< $(CPP) $(CFLAGS) -o $@ $(LIBS)