#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
/*#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unordered_map>
#include <vector>
#include <list>

#include "header/socket_helper.h"*/

#include "header/server.h"

using namespace std;
/*
void handleGetMessage(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* peerAddr = addr.addr_info;
	ssize_t n;
	socklen_t addrlen;
	struct metadata message_metadata;
	string message_content;
	char buffer[MAX_BUF];

	addrlen = sizeof(struct sockaddr_in);

    for (;;) 
	{
		memset(buffer, 0, strlen(buffer));
		n = recvfrom(messageFD, buffer, sizeof(buffer), 0, 
						(struct sockaddr*)peerAddr, &addrlen); 

		get_message_metadata(message_metadata, buffer);
		string message_content = get_message_content(buffer);
        cout<<	"Message received: " <<endl\
			<<	"\tClientID: "		<< message_metadata.client_id <<endl\
			<<	"\tMessageID: "		<< message_metadata.message_id <<endl\
			<<	"\tStatus: "		<< message_metadata.status_id <<endl\
			<<	"\tType: "			<< message_metadata.message_type_id <<endl\
			<< 	"\tContent: "		<< message_content <<endl;

		process_message(message_metadata, message_content);
		
		sendto(messageFD, (const char*)message_content.c_str(), message_content.length(),
				0, (struct sockaddr*)peerAddr, 
				addrlen);

		cout<<"\tResponse sent"<<endl;
    } 
}

void handleHeartbeat(struct addr_info& addr){
	int heartbeatFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    size_t n;
    socklen_t addrlen;
    char buffer[MAX_BUF];

    addrlen = sizeof(struct sockaddr_in);

    for(;;){
		memset(buffer, 0, strlen(buffer));
		
        n = recvfrom(heartbeatFD, (char*)buffer, MAX_BUF, 
                        0, (struct sockaddr*)servAddr, 
                        &addrlen);
        
        cout<<"Heartbeat received: "<<buffer<<endl; 


        sendto(heartbeatFD, (const char*)buffer, strlen(buffer), 
            0, (const struct sockaddr*)servAddr, 
            addrlen); 

        cout<<"\tHeartbeat response sent"<<endl;
    }
}
*/

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: %s port_number\n" << argv[0] << endl;
		return(1);
	}
	cout << "IPROTO_UDPLITE: " << IPPROTO_UDPLITE << endl;

	// Convert port number top integer 
	//int portNumber = atoi(argv[1]);

	Server *server = new Server(argc, argv);

	struct addr_info* messageInfo = createUdpLiteSocket(server->portNumber);
	struct addr_info* heartbeatInfo = createUdpLiteSocket(server->portNumber + 1);

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

	set_checksum_on_socket(messageInfo->fd, sizeof(struct metadata), UDPLITE_RECV_CSCOV);

    std::thread thread_getMessages(Server::handleGetMessage, server, ref(*messageInfo));
	std::thread thread_heartbeat(Server::handleHeartbeat, server, ref(*heartbeatInfo));

	thread_getMessages.join();
	thread_heartbeat.join();
	
	return 0;
}
