#pragma once
#include <fcntl.h>
#include <errno.h>

#include "core.hpp"

class Server : Core
{
public:
    int portNumber;
  
    Server(int argc, char* argv[]);
    void handleGetMessage(struct addr_info& addr);
    void handleHeartbeat(struct addr_info& addr);
};

Server::Server(int argc, char* argv[])
{
    this->portNumber = atoi(argv[1]);
	struct addr_info* messageInfo = createUdpLiteSocket(this->portNumber);
	struct addr_info* heartbeatInfo = createUdpLiteSocket(this->portNumber + 1);

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

    std::thread thread_getMessages(&Server::handleGetMessage, this, ref(*messageInfo));
	std::thread thread_heartbeat(&Server::handleHeartbeat, this, ref(*heartbeatInfo));

	thread_getMessages.join();
	thread_heartbeat.join();
}

void Server::handleGetMessage(struct addr_info& addr)
{
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

		if (n > -1)
		{
			get_message_metadata(message_metadata, buffer);
			string message_content = get_message_content(buffer);
			cout<<	"Message received: " <<endl\
				<<	"\tClientID: "		<< message_metadata.client_id <<endl\
				<<	"\tMessageID: "		<< message_metadata.message_id <<endl\
				<<	"\tStatus: "		<< message_metadata.status_id <<endl\
				<< 	"\tContent: "		<< message_content << endl;
			
			sendto(messageFD, (const char*)message_content.c_str(), message_content.length(),
					0, (struct sockaddr*)peerAddr, 
					addrlen);

			cout<<"\tResponse sent"<<endl;
		}
		else
		{
			cout<<"Receiving message FAILED! errno: "<<errno<<endl;
		}
    }
}

void Server::handleHeartbeat(struct addr_info& addr)
{
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
        
		if (n > -1)
		{
			cout<<"Heartbeat received: "<<buffer<<endl;


			sendto(heartbeatFD, (const char*)buffer, strlen(buffer), 
				0, (const struct sockaddr*)servAddr, 
				addrlen); 

			cout<<"\tHeartbeat response sent"<<endl;
		}
		else
		{
			cout<<"Heartbeat receive FAILED! errno: "<<errno<<endl;
		}
    }
}
