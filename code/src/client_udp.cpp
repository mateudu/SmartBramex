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

#include "header/client_udp.h"

using namespace std;


/*int client_id;
size_t message_id = 0;
status client_status = status_active;

void handleSendMessage(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    socklen_t addrlen;
    addrlen = sizeof(struct sockaddr_in);
    char buffer[MAX_BUF];

    struct metadata message_metadata;
    
    for(;;){
        memset(buffer, 0, MAX_BUF);
        message_id += 1;
        message_metadata.client_id = client_id;
        message_metadata.message_id = message_id;
        message_metadata.status_id = client_status;
        message_metadata.message_type_id = message_type_data;

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
*/

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

    struct addr_info* messageInfo = createUdpLiteSocket(client->portNumber, argv[1]);
    struct addr_info* heartbeatInfo = createUdpLiteSocket(client->portNumber + 1, argv[1]);

    set_checksum_on_socket(messageInfo->fd, sizeof(struct metadata), UDPLITE_SEND_CSCOV);
    set_timeout_on_socket(heartbeatInfo->fd, 33);

    std::thread thread_sendMessages(client->handleSendMessage, ref(*messageInfo));
    std::thread thread_getMessages(client->handleGetMessage, ref(*messageInfo));
    std::thread thread_heartbeat(client->handleHeartbeat, ref(*heartbeatInfo));
    
    thread_sendMessages.join();
    thread_getMessages.join();
    thread_heartbeat.join();
    
    close(messageInfo->fd); 
    close(heartbeatInfo->fd);
    return 0; 
} 
