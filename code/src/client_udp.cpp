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
    char message[] = "Hello Server"; 

    for(int i = 0;++i;i<10){
    // send hello message to server 
    sendto(messageFD, (const char*)message, strlen(message), 
           0, (const struct sockaddr*)servAddr, 
           sizeof(struct sockaddr_in)); 

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


    std::thread thread_sendMessages(handleSendMessage, ref(*messageInfo));
    std::thread thread_getMessages(handleGetMessage, ref(*messageInfo));
    //std::thread thread_heartbeat(handleHeartbeat, ref(heartbeatInfo));
    
    thread_sendMessages.join();
    thread_getMessages.join();
    
    close(messageInfo->fd); 
    close(heartbeatInfo->fd);
    return 0; 
} 
