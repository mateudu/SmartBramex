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

int client_id;
size_t message_id = 0;
Status client_status = active;

void handleSendMessage(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    socklen_t addrlen;
    addrlen = sizeof(struct sockaddr_in);
    char buffer[MAX_BUF];

    struct checksum message_metadata;
    
    for(;;){
        memset(buffer, 0, MAX_BUF);
        message_id += 1;
        message_metadata.clientID = client_id;
        message_metadata.messageID = message_id;
        message_metadata.statusID = active;

        generate_message(buffer, message_metadata, "abcdef");
        sendto(messageFD, (const char*)buffer, sizeof(buffer), 
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
    //Doesn't matter what the message is, we just want to send anything through
    std::string msg = "Heartbeat";
    char buffer[MAX_BUF];

    addrlen = sizeof(struct sockaddr_in);    
    
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
        
        cout<<"\tHeartbeat response received: "<<buffer<<endl; 

        sleep(8);
    }
}


int main(int argc, char* argv[]) 
{ 
    if (argc < 4)	{
		cout<<"Usage: %s ip_address port_number client_number "<<argv[0]<<endl;
		return(1);
	}

    
    int portNumber = atoi(argv[2]);
    client_id = stoi(argv[3]);

    struct addr_info* messageInfo = createUdpLiteSocket(portNumber, argv[1]);
    struct addr_info* heartbeatInfo = createUdpLiteSocket(portNumber + 1, argv[1]);

    set_checksum_on_socket(messageInfo->fd, sizeof(struct checksum), UDPLITE_SEND_CSCOV);
    set_timeout_on_socket(heartbeatInfo->fd, 33);

    std::thread thread_sendMessages(handleSendMessage, ref(*messageInfo));
    std::thread thread_getMessages(handleGetMessage, ref(*messageInfo));
    std::thread thread_heartbeat(handleHeartbeat, ref(*heartbeatInfo));
    
    thread_sendMessages.join();
    thread_getMessages.join();
    thread_heartbeat.join();
    
    close(messageInfo->fd); 
    close(heartbeatInfo->fd);
    return 0; 
} 
