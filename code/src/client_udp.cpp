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

#define MAXLINE 1024 
#define PORT 6969 

using namespace std;

int main(int argc, char* argv[]) 
{ 
    int sockfd; 
    char buffer[MAXLINE]; 
    char message[] = "Hello Server"; 
    struct sockaddr_in servaddr; 

    socklen_t n, len; 

    if (argc < 3)	{
		cout<<"Usage: %s ip_address port_number "<<argv[0]<<endl;
		return(1);
	}

    // Creating socket file descriptor 
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE);
	if (sockfd == -1){
		cout<<"Socket creation"<<endl;
		return(1);
	}
  
    memset(&servaddr, 0, sizeof(servaddr)); 
  
    // Filling server information 
    servaddr.sin_family = AF_INET;
    inet_aton(argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(stoi(argv[2]));


    // send hello message to server 
    sendto(sockfd, (const char*)message, strlen(message), 
           0, (const struct sockaddr*)&servaddr, 
           sizeof(struct sockaddr_in)); 
  
    // receive server's response 
    cout<<"Message from server: "<<endl; 
    n = recvfrom(sockfd, (char*)buffer, MAXLINE, 
                 0, (struct sockaddr*)&servaddr, 
                 &len); 
    cout<<buffer<<endl; 
    close(sockfd); 
    return 0; 
} 
