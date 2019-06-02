/* Projekt: SmartBramex
-- Funkcja: Logika dla klienta - czujnika
-- Ostatnia zmiana: 01/06/2019
-- Autorzy: Piotr Ogrodnik, Krzysztof Kasprzak, Sebastian Kamoda */

#pragma once
#include <fcntl.h>
#include <errno.h>
#include <map>

#include "core.hpp"

class Server : Core
{
public:
    Server(int argc, char* argv[]);

	void process_message(struct message&) override;
    void handleGetMessage() override;
    void handleHeartbeat() override;

private:
	map<int, struct client_info> clients;
	std::mutex mutex;
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
    struct sockaddr_in* peerAddr = message_addr_info->addr_info;
	ssize_t n;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	struct message msg;
	char buffer[MAX_BUF];

    for (;;) 
	{
		memset(buffer, 0, strlen(buffer));
		n = recvfrom(message_addr_info->fd, buffer, sizeof(buffer), 0, 
						(struct sockaddr*)peerAddr, &addrlen); 

		if (n == -1) {
			cout<<"Receive message FAILED! errno: "<<errno<<endl;
		}
		else {
			get_message_metadata(msg.metadata, buffer);
			msg.content = get_message_content(buffer);
			cout<<	"Message received: " <<endl\
				<<	"\tClientID: "		<< msg.metadata.client_id <<endl\
				<<	"\tMessageID: "		<< msg.metadata.message_id <<endl\
				<<	"\tStatus: "		<< msg.metadata.status_id <<endl\
				<<	"\tType: "			<< msg.metadata.message_type_id <<endl\
				<< 	"\tContent: "		<< msg.content <<endl;

			process_message(msg);

			std::unique_lock<std::mutex> lk(mutex);
			if (clients[msg.metadata.client_id].missing_messages_id.size() > 0)
			{
				msg.metadata.message_id = clients[msg.metadata.client_id].missing_messages_id.front();
				msg.metadata.message_type_id = message_type_data_request;
				msg.content.clear(); 
				send_message(msg, message_addr_info->fd, peerAddr);
				cout<<"Message request has been sent to client: "<<msg.metadata.client_id <<"Message ID: "<<msg.metadata.message_id<<endl;
			}
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

void Server::process_message(struct message& msg)
{
	int client_id = msg.metadata.client_id;
	size_t message_id = msg.metadata.message_id;

	switch (msg.metadata.message_type_id)
	{
	case message_type_data:
		mutex.lock();
		if (clients[client_id].expected_message_id != message_id && message_id != 1){
			cout << "Registered a missing message. Message ID: " << clients[client_id].expected_message_id << endl;
			//we missed a message; add it to the missing list
			clients[client_id].missing_messages_id.push_back(clients[client_id].expected_message_id);
		}

		//do sth
		clients[client_id].expected_message_id = message_id + 1;
		mutex.unlock();
		break;
	case message_type_data_request_response:
		cout << "Received a missing message. Message ID: " << msg.metadata.message_id << endl;
		//We have received a missing message so we no longer need it
		
		mutex.lock();
		clients[msg.metadata.client_id].missing_messages_id.remove(msg.metadata.message_id);
		mutex.unlock();

		break;
	default:
		throw "Unknown/Unhandled message type!";
		break;
	}
}
