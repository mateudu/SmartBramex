#pragma once
#include <arpa/inet.h>

#include "core.hpp"

class Client_udp : public Core
{
public:  
  int portNumber;
  int client_id;
  size_t message_id;
  status client_status;

public:
  Client_udp(int argc, char* argv[]);
  void handleSendMessage(struct addr_info& addr);
  void handleGetMessage(struct addr_info& addr);
  void handleHeartbeat(struct addr_info& addr);
};

Client_udp::Client_udp(int argc, char* argv[])
{
    this->portNumber = atoi(argv[2]);
    this->client_id = stoi(argv[3]);
    this->message_id = 0;
    this->client_status = status_active;

	struct addr_info* messageInfo = createUdpLiteSocket(this->portNumber, argv[1]);
    struct addr_info* heartbeatInfo = createUdpLiteSocket(this->portNumber + 1, argv[1]);

    set_checksum_on_socket(messageInfo->fd, sizeof(struct metadata), UDPLITE_SEND_CSCOV);
    set_timeout_on_socket(heartbeatInfo->fd, 33);

    std::thread thread_sendMessages(&Client_udp::handleSendMessage, this, ref(*messageInfo));
    std::thread thread_getMessages(&Client_udp::handleGetMessage, this, ref(*messageInfo));
    std::thread thread_heartbeat(&Client_udp::handleHeartbeat, this, ref(*heartbeatInfo));
    
    thread_sendMessages.join();
    thread_getMessages.join();
    thread_heartbeat.join();
    
    close(messageInfo->fd); 
    close(heartbeatInfo->fd);
}

void Client_udp::handleSendMessage(struct addr_info& addr)
{
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

void Client_udp::handleGetMessage(struct addr_info& addr)
{
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
        if (n == -1)
        {
            cout<<"Receiving message from server FAILED! errno: "<<errno<<" n: "<<n<<endl;
        }
        else
        {
            cout<<"Message from server: "<<buffer<<endl;
        }
    }
}

void Client_udp::handleHeartbeat(struct addr_info& addr)
{
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
