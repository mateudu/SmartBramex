#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <stdlib.h>
#include <string.h>

#include "socket_helper.h"

using namespace std;

class Core
{
public:
  void handleGetMessage(struct addr_info& addr);
  void handleHeartbeat(struct addr_info& addr);
};
