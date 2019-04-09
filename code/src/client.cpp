#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUF 100

using namespace std;

int main(int argc, char* argv[])
{
	int sockd;
	int count;
	struct sockaddr_in serv_name;
	char buf[MAX_BUF];
	int status;

	if (argc < 3)
	{
		cout<<"Usage: %s ip_address port_number "<<argv[0]<<endl;
		return(1);
	}
	/* create a socket */
	sockd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockd == -1)
	{
		cout<<"Socket creation"<<endl;
		return(1);
	}

	/* server address */ 
	serv_name.sin_family = AF_INET;
	inet_aton(argv[1], &serv_name.sin_addr);
	serv_name.sin_port = htons(stoi(argv[2]));

	/* connect to the server */
	status = connect(sockd, (struct sockaddr*)&serv_name, sizeof(serv_name));
	if (status == -1)
	{
		cout<<"Connection error"<<endl;
		return(1);
	}

	count = read(sockd, buf, MAX_BUF);
	write(1, buf, count);

	close(sockd);
	return 0;
}
