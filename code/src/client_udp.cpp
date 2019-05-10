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
//temporary
std::string messageToSend;

void handleSendMessage(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    socklen_t addrlen;

    addrlen = sizeof(struct sockaddr_in);
    
    for(;;){
        sendto(messageFD, (const char*)messageToSend.c_str(), messageToSend.length(), 
            0, (const struct sockaddr*)servAddr, 
            addrlen); 

        cout<<"Message sent"<<endl;

        sleep(5);
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
    //temporary
    std::string msg = "Heartbeat: ";
    msg += messageToSend;
    //temporary
    char buffer[MAX_BUF];

    addrlen = sizeof(struct sockaddr_in);

    struct timeval tv;
    tv.tv_sec = 15;
    tv.tv_usec = 0;
    
    for(;;){
        // send hello message to server 
        sendto(heartbeatFD, (const char*)msg.c_str(), msg.length(), 
            0, (const struct sockaddr*)servAddr, 
            addrlen); 

        cout<<"Heartbeat sent"<<endl;

        n = recvfrom(heartbeatFD, (char*)buffer, MAX_BUF, 
                        0, (struct sockaddr*)servAddr, 
                        &addrlen);
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            cout<<"CONNECTION LOST!!"<<endl;
            throw "Connection lost";
        }
        
        cout<<"Heartbeat got: "<<buffer<<endl; 

        sleep(1);
    }
}



int main(int argc, char* argv[]) 
{ 
    if (argc < 4)	{
		cout<<"Usage: %s ip_address port_number client_number "<<argv[0]<<endl;
		return(1);
	}

    
    int portNumber = atoi(argv[2]);

    std::string clientId = argv[3];
    messageToSend = "Client " + clientId;

    struct chcksum* checksum = new chcksum;
    checksum->clientID = clientId;
    checksum->messageID = -1;

    struct addr_info* messageInfo = createUdpLiteSocket(portNumber, argv[1], sizeof(&checksum));
    struct addr_info* heartbeatInfo = createUdpLiteSocket(portNumber + 1, argv[1],0);

    std::thread thread_sendMessages(handleSendMessage, ref(*messageInfo));
    std::thread thread_getMessages(handleGetMessage, ref(*messageInfo));
    std::thread thread_heartbeat(handleHeartbeat, ref(*heartbeatInfo));
    
    thread_sendMessages.join();
    thread_getMessages.join();
    
    close(messageInfo->fd); 
    close(heartbeatInfo->fd);
    return 0; 
} 
