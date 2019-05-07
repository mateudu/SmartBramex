CC=g++
CFLAGS=-g -std=c++11 -pthread
BIN=./bin/
SOURCE=./src/

LIST=$(BIN)server $(BIN)client_udp

all: $(LIST)

$(BIN)%:  $(SOURCE)%.cpp
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)