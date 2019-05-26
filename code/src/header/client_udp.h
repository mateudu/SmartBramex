#include <arpa/inet.h>

#include "core.h"

class Client_udp : public Core
{
public:  
  int portNumber;
  int client_id;
  size_t message_id;
  Status client_status;

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
    this->client_status = active;
}

void Client_udp::handleSendMessage(struct addr_info& addr)
{
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

        cout<<"Message from server: "<<buffer<<endl; 
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