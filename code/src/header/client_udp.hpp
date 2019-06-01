#pragma once
#include <arpa/inet.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "core.hpp"
#define SENT_MESSAGES_BUFFER_SIZE 128

class Client_udp : public Core
{
public:
    int client_id;
    size_t message_id;
    size_t start_message_id;
    status client_status;
    int interval;
    int jitter;

    Client_udp(int argc, char* argv[]);
    void handleSendMessage();

    void handleGetMessage() override;
    void handleHeartbeat() override;
    void process_message(struct message&);

private:
    vector<message> sent_messages;
	std::mutex mutex;
	string data_generator();
};

Client_udp::Client_udp(int argc, char* argv[])
{
    this->portNumber = atoi(argv[2]);
    this->client_id = stoi(argv[3]);
    this->interval = atoi(argv[4]);
    this->jitter = atoi(argv[5]);
    this->start_message_id = atoi(argv[6]);
    this->message_id = start_message_id;
    this->client_status = status_active;

	message_addr_info = createUdpLiteSocket(this->portNumber, argv[1]);
    heartbeat_addr_info = createUdpLiteSocket(this->portNumber + 1, argv[1]);

    set_checksum_on_socket(message_addr_info->fd, sizeof(struct metadata), UDPLITE_SEND_CSCOV);
    set_timeout_on_socket(heartbeat_addr_info->fd, 33);

    std::thread thread_sendMessages(&Client_udp::handleSendMessage, this);
    std::thread thread_getMessages(&Client_udp::handleGetMessage, this);
    std::thread thread_heartbeat(&Client_udp::handleHeartbeat, this);
    
    thread_sendMessages.join();
    thread_getMessages.join();
    thread_heartbeat.join();
    
    close(message_addr_info->fd); 
    close(heartbeat_addr_info->fd);
}

string Client_udp::data_generator()
{
    srand(time(NULL));
    float msg = rand() % 31 + 10; //10-30

    if(rand()%1 == 0)
    {
        float jitter = rand() % 10;
        jitter /= 6;
        msg *= jitter;
    }
    return to_string(msg);
}

void Client_udp::handleSendMessage()
{
    struct message msg;

    for(;;){
        message_id += 1;
        msg.metadata.client_id = client_id;
        msg.metadata.message_id = message_id;
        msg.metadata.status_id = client_status;
        msg.metadata.message_type_id = message_type_data;
        msg.content = data_generator();

        send_message(msg, message_addr_info->fd, message_addr_info->addr_info);

        sent_messages.push_back(msg);
        
        //Check for vector size and remove unnecessary messages
	    mutex.lock();
        if (sent_messages.size() >= SENT_MESSAGES_BUFFER_SIZE * 2)
        {
            sent_messages.erase(sent_messages.begin(), sent_messages.begin() + SENT_MESSAGES_BUFFER_SIZE);
        }
        mutex.unlock();
        
        cout<<"Message sent"<<endl;
        sleep(interval);
    }
}

void Client_udp::handleGetMessage()
{
    int messageFD = message_addr_info->fd;
    struct sockaddr_in* servAddr = message_addr_info->addr_info;
    size_t n;
    socklen_t addrlen;
    char buffer[MAX_BUF];
    struct message msg;

    addrlen = sizeof(struct sockaddr_in);

    for(;;){
        n = recvfrom(messageFD, (char*)buffer, MAX_BUF, 
                  0, (struct sockaddr*)servAddr, 
                  &addrlen);
        if (n == -1)
        {
            cout<<"Receiving message from server FAILED! errno: "<<errno<<" n: "<<n<<endl;
        }
        else
        {
            get_message_metadata(msg.metadata, buffer);
			msg.content = get_message_content(buffer);
			cout<<	"Message received: " <<endl\
				<<	"\tClientID: "		<< msg.metadata.client_id <<endl\
				<<	"\tMessageID: "		<< msg.metadata.message_id <<endl\
				<<	"\tStatus: "		<< msg.metadata.status_id <<endl\
				<<	"\tType: "			<< msg.metadata.message_type_id <<endl\
				<< 	"\tContent: "		<< msg.content <<endl;

			process_message(msg);
        }
    }
}

void Client_udp::handleHeartbeat()
{
    int heartbeatFD = heartbeat_addr_info->fd;
    struct sockaddr_in* servAddr = heartbeat_addr_info->addr_info;
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

void Client_udp::process_message(struct message& msg)
{
    switch (msg.metadata.message_type_id) {
        case message_type_data_request:
            //Find message with that ID from vector of messages sent
            mutex.lock();
            for (auto it : sent_messages)
            {
                if (it.metadata.message_id == msg.metadata.message_id)
                {
                    msg = it;
                    break;
                }                
            }
            mutex.unlock();
            
            msg.metadata.client_id = client_id;
            msg.metadata.status_id = client_status;
            msg.metadata.message_type_id = message_type_data_request_response;

            send_message(msg, message_addr_info->fd, message_addr_info->addr_info);

            cout<<"Could not find the message with ID: "<<message_id<<endl;
            break;
        default:
            throw "Unknown/Unhandled message type!";
            break;
	}
}