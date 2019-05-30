/*#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

#include "header/socket_helper.h"*/

#include "header/client_udp.hpp"

using namespace std;

int main(int argc, char* argv[]) 
{ 
    if (argc < 4)	{
		cout<<"Usage: %s ip_address port_number client_number "<<argv[0]<<endl;
		return(1);
	}

    /*Client_udp *client = new Client_udp();
    client->portNumber = atoi(argv[2]);
    client->client_id = stoi(argv[3]);*/

    Client_udp *client = new Client_udp(argc, argv);

    return 0; 
} 
