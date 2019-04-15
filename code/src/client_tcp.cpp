//TODO:Zmodularyzować do funkcji całe działanie klienta
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

#define MAX_BUF 1024

using namespace std;

int main(int argc, char* argv[])
{
	int sockfd;
	int count;
	struct sockaddr_in servaddr;
	char buf[MAX_BUF];
	int status;

	if (argc < 3)	{
		cout<<"Usage: %s ip_address port_number "<<argv[0]<<endl;
		return(1);
	}
	
    // Creating socket file descriptor 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		cout<<"Socket creation"<<endl;
		return(1);
	}

	/* server address */ 
	servaddr.sin_family = AF_INET;
	inet_aton(argv[1], &servaddr.sin_addr);
	servaddr.sin_port = htons(stoi(argv[2]));

	/* connect to the server */
	status = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (status == -1)
	{
		cout<<"Connection error"<<endl;
		return(1);
	}

	count = read(sockfd, buf, MAX_BUF);
	write(1, buf, count);

	close(sockfd);
	return 0;
}
