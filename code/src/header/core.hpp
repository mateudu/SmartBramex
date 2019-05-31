#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <stdlib.h>
#include <string.h>

#include "socket_helper.hpp"
#include "semaphore.hpp"

using namespace std;

class Core
{
public:  
	virtual void handleGetMessage() {}
	virtual void handleHeartbeat() {}
	virtual void process_message(struct message&) {}
  
	struct addr_info* message_addr_info;
	struct addr_info* heartbeat_addr_info;
	int portNumber;
};