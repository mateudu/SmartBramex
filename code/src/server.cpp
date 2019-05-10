#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "header/socket_helper.h"

using namespace std;


void handleGetMessage(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* peerAddr = addr.addr_info;
	ssize_t n;
	socklen_t addrlen;
	char buffer[MAX_BUF];

	addrlen = sizeof(struct sockaddr_in);

    for (;;) 
	{
		memset(buffer, 0, strlen(buffer));
		n = recvfrom(messageFD, buffer, sizeof(buffer), 0, 
						(struct sockaddr*)peerAddr, &addrlen); 

        cout<<"Message got: "<<buffer<<endl;
		
		sendto(messageFD, (const char*)buffer, sizeof(buffer), 0, 
		 		(struct sockaddr*)peerAddr, addrlen);

		cout<<"Message sent"<<endl;
    } 
}

void handleHeartbeat(struct addr_info& addr){
	int heartbeatFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    size_t n;
    socklen_t addrlen;
    char message[] = "Heartbeat: Server"; 
    char buffer[MAX_BUF];

    addrlen = sizeof(struct sockaddr_in);

    for(;;){
		memset(buffer, 0, strlen(buffer));
		
        n = recvfrom(heartbeatFD, (char*)buffer, MAX_BUF, 
                        0, (struct sockaddr*)servAddr, 
                        &addrlen);
        
        cout<<"Heartbeat got: "<<buffer<<endl; 


        sendto(heartbeatFD, (const char*)buffer, strlen(buffer), 
            0, (const struct sockaddr*)servAddr, 
            addrlen); 

        cout<<"Heartbeat sent"<<endl;
    }
}



int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: %s port_number\n" << argv[0] << endl;
		return(1);
	}
	cout << "IPROTO_UDPLITE: " << IPPROTO_UDPLITE << endl;

	// Convert port number top integer 
	int portNumber = atoi(argv[1]);

	struct addr_info* messageInfo = createUdpLiteSocket(portNumber);
	struct addr_info* heartbeatInfo = createUdpLiteSocket(portNumber + 1);

    // binding server addr structure to udp sockfd 
    int status = bind(messageInfo->fd, (struct sockaddr*)messageInfo->addr_info, sizeof(struct sockaddr_in));
	if (status == -1) {
		cout<<"UDP binding error: "<< errno <<endl;
		throw "UDP binding error";
	}

    status = bind(heartbeatInfo->fd, (struct sockaddr*)heartbeatInfo->addr_info, sizeof(struct sockaddr_in));
	if (status == -1) {
		cout<<"UDP binding error: "<< errno <<endl;
		throw "UDP binding error";
	}


    std::thread thread_getMessages(handleGetMessage, ref(*messageInfo));
	std::thread thread_heartbeat(handleHeartbeat, ref(*heartbeatInfo));

	thread_getMessages.join();
	thread_heartbeat.join();
	
	return 0;
}