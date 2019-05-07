#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

#include "header/socket_helper.h"

using namespace std;


void handleSendMessage(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    socklen_t addrlen;
    char message[] = "Hello Server"; 

    addrlen = sizeof(struct sockaddr_in);

    for(int i = 0;++i;i<10){
    // send hello message to server 
    sendto(messageFD, (const char*)message, strlen(message), 
           0, (const struct sockaddr*)servAddr, 
           addrlen); 

    cout<<"Message sent"<<endl;

    sleep(3);
  
    }
}

void handleGetMessage(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    size_t n;
    socklen_t addrlen;
    char buffer[MAX_BUF];

    addrlen = sizeof(struct sockaddr_in);

    for(;;){
        n = recvfrom(messageFD, (char*)buffer, MAX_BUF, 
                    0, (struct sockaddr*)servAddr, 
                    &addrlen);

        cout<<"Message from server: "<<buffer<<endl; 
    }
}

void handleHeartbeat(struct addr_info& addr){
    int heartbeatFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    size_t n;
    socklen_t addrlen;
    char message[] = "Heartbeat: Client"; 
    char buffer[MAX_BUF];

    addrlen = sizeof(struct sockaddr_in);

    struct timeval tv;
    tv.tv_sec = 15;
    tv.tv_usec = 0;
    setsockopt(heartbeatFD, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    for(;;){
        // send hello message to server 
        sendto(heartbeatFD, (const char*)message, strlen(message), 
            0, (const struct sockaddr*)servAddr, 
            addrlen); 

        cout<<"heartbeatSent"<<endl;

        n = recvfrom(heartbeatFD, (char*)buffer, MAX_BUF, 
                        0, (struct sockaddr*)servAddr, 
                        &addrlen);
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            cout<<"CONNECTION LOST!!"<<endl;
            throw "Connection lost";
        }
        
        cout<<"heartbeatGot: "<<buffer<<endl; 

        sleep(1);
    }
}



int main(int argc, char* argv[]) 
{ 
    if (argc < 3)	{
		cout<<"Usage: %s ip_address port_number "<<argv[0]<<endl;
		return(1);
	}

    
    int portNumber = atoi(argv[2]);

    struct addr_info* messageInfo = createUdpLiteSocket(portNumber, argv[1]);
    struct addr_info* heartbeatInfo = createUdpLiteSocket(portNumber + 1, argv[1]);

    // int status = bind(messageInfo->fd, (struct sockaddr*)messageInfo->addr_info, sizeof(struct sockaddr_in));
	// if (status == -1) {
	// 	cout<<"UDP binding error: "<< errno <<endl;
	// 	throw "UDP binding error";
	// }

    // status = bind(heartbeatInfo->fd, (struct sockaddr*)heartbeatInfo->addr_info, sizeof(struct sockaddr_in));
	// if (status == -1) {
	// 	cout<<"UDP binding error: "<< errno <<endl;
	// 	throw "UDP binding error";
	// }

    std::thread thread_sendMessages(handleSendMessage, ref(*messageInfo));
    std::thread thread_getMessages(handleGetMessage, ref(*messageInfo));
    std::thread thread_heartbeat(handleHeartbeat, ref(*heartbeatInfo));
    
    thread_sendMessages.join();
    thread_getMessages.join();
    
    close(messageInfo->fd); 
    close(heartbeatInfo->fd);
    return 0; 
} 
