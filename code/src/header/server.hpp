#pragma once
#include <fcntl.h>
#include <errno.h>
#include <unordered_map>

#include "core.hpp"

class Server : Core
{
public:
    int portNumber;
  
    Server(int argc, char* argv[]);
    void handleGetMessage();
    void handleHeartbeat();
	void process_message(struct metadata& metadata_struct, string& content);

private:
	unordered_map<int, struct client_info> clients;
	struct addr_info* message_addr_info;
	struct addr_info* heartbeat_addr_info;
};

Server::Server(int argc, char* argv[])
{
    this->portNumber = atoi(argv[1]);
	message_addr_info = createUdpLiteSocket(this->portNumber);
	heartbeat_addr_info = createUdpLiteSocket(this->portNumber + 1);

    // binding server addr structure to udp sockfd 
    int status = bind(message_addr_info->fd, (struct sockaddr*)message_addr_info->addr_info, sizeof(struct sockaddr_in));
	if (status == -1) {
		cout<<"UDP binding error: "<< errno <<endl;
		throw "UDP binding error";
	}

    status = bind(heartbeat_addr_info->fd, (struct sockaddr*)heartbeat_addr_info->addr_info, sizeof(struct sockaddr_in));
	if (status == -1) {
		cout<<"UDP binding error: "<< errno <<endl;
		throw "UDP binding error";
	}

	set_checksum_on_socket(message_addr_info->fd, sizeof(struct metadata), UDPLITE_RECV_CSCOV);

    std::thread thread_getMessages(&Server::handleGetMessage, this);
	std::thread thread_heartbeat(&Server::handleHeartbeat, this);

	thread_getMessages.join();
	thread_heartbeat.join();
}

void Server::handleGetMessage()
{
	int messageFD = message_addr_info->fd;
    struct sockaddr_in* peerAddr = message_addr_info->addr_info;
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

		if (n == -1) {
			cout<<"Receive message FAILED! errno: "<<errno<<endl;
		}
		else {
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
}

void Server::handleHeartbeat()
{
	int heartbeatFD = heartbeat_addr_info->fd;
    struct sockaddr_in* servAddr = heartbeat_addr_info->addr_info;
    size_t n;
    socklen_t addrlen;
    char buffer[MAX_BUF];

    addrlen = sizeof(struct sockaddr_in);

    for(;;){
		memset(buffer, 0, strlen(buffer));

        n = recvfrom(heartbeatFD, (char*)buffer, MAX_BUF, 
                        0, (struct sockaddr*)servAddr, 
                        &addrlen);

		if (n == -1) {
			cout<<"Heartbeat receive FAILED! errno: "<<errno<<endl;
		}
		else {
			cout<<"Heartbeat received: "<<buffer<<endl; 

			sendto(heartbeatFD, (const char*)buffer, strlen(buffer), 
				0, (const struct sockaddr*)servAddr, 
				addrlen); 

			cout<<"\tHeartbeat response sent"<<endl;
		}
		
		
        
    }
}

void Server::process_message(struct metadata& metadata_struct, string& content)
{
	int client_id = metadata_struct.client_id;
	size_t message_id = metadata_struct.message_id;

	switch (metadata_struct.message_type_id)
	{
	case message_type_data:
		if (clients[client_id].expected_message_id != message_id && message_id != 1){
			cout << "Registered a missing message. Message ID: " << clients[client_id].expected_message_id << endl;
			//we missed a message; add it to the missing list
			clients[client_id].missing_messages_id.push_back(clients[client_id].expected_message_id);
		}

		//do sth
		clients[client_id].expected_message_id = message_id + 1;
		break;

	case message_type_data_request_response:
		cout << "Received a missing message. Message ID: " << metadata_struct.message_id << endl;
		//We have received a missing message so we no longer need it
		clients[metadata_struct.client_id].missing_messages_id.remove(metadata_struct.message_id);
		break;

	default:
		throw "Unknown/Unhandled message type!";
		break;
	}
}
