//TODO:
//-Zrobic zeby sie kompilowalo, ale zeby wygladalo podbnie jak klient tcp
//-zmodularyzować całe działanie klienta. Tak zeby calosc byla w funkcji i mozna bylo odpalic to z maina bezposrednio
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

#define MAXLINE 1024 
#define PORT 6969 

using namespace std;

struct addr_info{
    struct sockaddr_in* addr_info;
    int fd;
};

void handleSendMessageThread(struct addr_info& addr){
    int messageFD = addr.fd;
    struct sockaddr_in* servAddr = addr.addr_info;
    char message[] = "Hello Server"; 


    // send hello message to server 
    sendto(messageFD, (const char*)message, strlen(message), 
           0, (const struct sockaddr*)servAddr, 
           sizeof(struct sockaddr_in)); 
  
    // receive server's response 
    cout<<"Message from server: "<<endl; 
    socklen_t n, len;
    char buffer[MAXLINE];

    n = recvfrom(messageFD, (char*)buffer, MAXLINE, 
                 0, (struct sockaddr*)servAddr, 
                 &len);

    cout<<buffer<<endl;
}

void handleGetMessageThread(struct addr_info &addr){

}

void handleHeartbeat(struct addr_info &addr){

}



int main(int argc, char* argv[]) 
{ 
    int messageFD, heartbeatFD;
    struct sockaddr_in messageServAddr, heartbeatServAddr;


    if (argc < 3)	{
		cout<<"Usage: %s ip_address port_number "<<argv[0]<<endl;
		return(1);
	}

    // Creating message file descriptor 
    messageFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE);
	if (messageFD == -1){
		cout<<"Socket creation"<<endl;
		return(1);
	}
  
    memset(&messageServAddr, 0, sizeof(messageServAddr)); 
  
    messageServAddr.sin_family = AF_INET;
    inet_aton(argv[1], &messageServAddr.sin_addr);
	messageServAddr.sin_port = htons(stoi(argv[2]));

    struct addr_info messageInfo = {&messageServAddr, messageFD};

    // Creating heartbeat file descriptor 
    heartbeatFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE);
	if (heartbeatFD == -1){
		cout<<"Socket creation"<<endl;
		return(1);
	}
  
    memset(&heartbeatServAddr, 0, sizeof(heartbeatServAddr)); 
  
    heartbeatServAddr.sin_family = AF_INET;
    inet_aton(argv[1], &heartbeatServAddr.sin_addr);
	heartbeatServAddr.sin_port = htons(stoi(argv[2]) + 1);

    struct addr_info heartbeatInfo = {&heartbeatServAddr, heartbeatFD};

    std::thread thread_sendMessages(handleSendMessageThread, ref(messageInfo));
    std::thread thread_getMessages(handleGetMessageThread, ref(messageInfo));
    std::thread thread_heartbeat(handleHeartbeat, ref(heartbeatInfo));
    
    thread_sendMessages.join();
    
    close(messageFD); 
    return 0; 
} 
