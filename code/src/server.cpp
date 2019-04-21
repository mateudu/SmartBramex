#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

using namespace std;

#define MAX_BUF 1024
//TODO:Troche parametryzacjy


void handle_new_client(int socket){
	for(;;){//TODO: add signal handling to end child process
		//TODO: add heartbeat
	}
	return;
}



int main(int argc, char* argv[])
{
	int listenfd, connfd, udpfd, maxfdp1, nready;
	fd_set rset;
	ssize_t n;
	socklen_t addrlen;
	struct sockaddr_in servaddr, peer_name;
	int status;
	char buffer[MAX_BUF];
	
	char message[] = "aaaa";

	if (argc < 2)
	{
		cout << "Usage: %s port_number\n" << argv[0] << endl;
		return(1);
	}
	cout << "IPROTO_UDPLITE: " << IPPROTO_UDPLITE << endl;
	// Create UDP socket
	udpfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDPLITE);
	if (udpfd == -1){
		cout << "UDP socket creation error: "<< errno << endl;
		return(1);
	}

	// Convert port number top integer 
	int portNumber = stoi(argv[1]);
	
	// Set server address 
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(portNumber);

    // binding server addr structure to udp sockfd 
    status = bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (status == -1) {
		cout<<"UDP binding error: "<< errno <<endl;
		return(1);
	}

	for (;;) 
	{
		addrlen = sizeof(peer_name); 
		memset(buffer, 0, sizeof(buffer)); 
		cout<<"\nMessage from UDP client: "<<endl;
		n = recvfrom(udpfd, buffer, sizeof(buffer), 0, 
						(struct sockaddr*)&peer_name, &addrlen); 
		cout<<buffer<<endl; 
		// sendto(udpfd, (const char*)message, sizeof(buffer), 0, 
		// 		(struct sockaddr*)&peer_name, sizeof(peer_name));
    } 
	return 0;
}