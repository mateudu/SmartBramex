CC=g++
CFLAGS=-g
BIN=./bin/
SOURCE=./src/

LIST=$(BIN)server $(BIN)client_tcp $(BIN)client_udp

all: $(LIST)

$(BIN)%:  $(SOURCE)%.cpp
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)