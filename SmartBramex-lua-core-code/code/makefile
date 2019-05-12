CC=g++
CFLAGS=-g
BIN=./bin/
SOURCE=./src/

LIST=$(BIN)server $(BIN)client

all: $(LIST)

$(BIN)%:  $(SOURCE)%.cpp
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)