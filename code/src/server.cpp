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

using namespace std;

#define MAX_BUF 1024
//TODO:Troche parametryzacjy
//Potestowac to troche bo w sumie nie wiem czy dziala xdd
//SOURCE: https://www.geeksforgeeks.org/tcp-and-udp-server-using-select/


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
	int no_child_processes = 0;
	pid_t child_processes[16];
	
	char message[] = "aaaa";

	if (argc < 2)
	{
		cerr << "Usage: %s port_number\n" << argv[0] << endl;
		return(1);
	}

	/* Create TCP socket */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	
	{
		cerr << "TCP socket creation error" << endl;
		return(1);
	}

	/* Convert port number top integer */
	int portNumber = stoi(argv[1]);
	
	/* Set server address */
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(stoi(argv[1]));

	/* Bind TCP address */
	if ((status = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) == -1) {
		cerr << "TCP binding error" << endl;
		return(1);
	}

	/* Set TCP listen */
	if ((status = listen(listenfd, 10)) == -1) {
		cerr << "TCP listening error" << endl;
		return(1);
	}
	
	/* Creating UDP socket before TCP socket (it throws -1) */
	if ((udpfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE)) == -1) 
	{
		cerr << "UDP socket creation error" << endl;
		return(1);
	}

    // binding server addr structure to udp sockfd 
    status = bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (status == -1) {
		cout<<"UDP binding error"<<endl;
		return(1);
	} 
  
    // clear the descriptor set 
    FD_ZERO(&rset); 
  
    // get maxfd 
    maxfdp1 = max(listenfd, udpfd) + 1; 


	for (;;) 
	{ 
        // set listenfd and udpfd in readset 
        FD_SET(listenfd, &rset); 
        FD_SET(udpfd, &rset); 
  
        // select the ready descriptor 
        nready = select(maxfdp1, &rset, NULL, NULL, NULL); 
  
        // if tcp socket is readable then handle 
        // it by accepting the connection 
        if (FD_ISSET(listenfd, &rset)) { 
            addrlen = sizeof(peer_name); 
            connfd = accept(listenfd, (struct sockaddr*)&peer_name, &addrlen);
			pid_t child = fork();
            if (child == 0) {
				write(connfd, "Hello!\n", 7);
                handle_new_client(connfd);
                close(connfd);
            } 
			else if(child > 0)
			{
				child_processes[no_child_processes] = child;
				++no_child_processes;
			}
			else
			{
				//TODO:
			} 
        } 
        // if udp socket is readable receive the message. 
        if (FD_ISSET(udpfd, &rset)) { 
            addrlen = sizeof(peer_name); 
            memset(buffer, 0, sizeof(buffer)); 
            cout<<"\nMessage from UDP client: "<<endl;
            n = recvfrom(udpfd, buffer, sizeof(buffer), 0, 
                         (struct sockaddr*)&peer_name, &addrlen); 
            cout<<buffer<<endl; 
            sendto(udpfd, (const char*)message, sizeof(buffer), 0, 
                    (struct sockaddr*)&peer_name, sizeof(peer_name)); 
        } 
    } 
	return 0;
}