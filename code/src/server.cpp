#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[])
{
	int sockd, sockd2;
	socklen_t addrlen;
	struct sockaddr_in my_name, peer_name;
	int status;

	/* create a socket */
	sockd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockd == -1)
	{
		cout<<"Socket creation error"<<endl;
		return(1);
	}

	if (argc < 2)
	{
		cout<<"Usage: %s port_number\n"<<argv[0]<<endl;
		return(1);
	}

	/* server address  */
	my_name.sin_family = AF_INET;
	my_name.sin_addr.s_addr = INADDR_ANY;
	my_name.sin_port = htons(stoi(argv[1]));

	status = bind(sockd, (struct sockaddr*)&my_name, sizeof(my_name));
	if (status == -1)
	{
		cout<<"Binding error"<<endl;
		return(1);
	}

	status = listen(sockd, 5);
	if (status == -1)
	{
		cout<<"Listening error"<<endl;
		return(1);
	}

	for(;;)
	{
		/* wait for a connection */
		addrlen = sizeof(peer_name);
		sockd2 = accept(sockd, (struct sockaddr*)&peer_name, &addrlen);
		if (sockd2 == -1)
		{
			cout<<"Wrong connection"<<endl;
			return(1);
		}
		write(sockd2, "Hello!\n", 7);
		close(sockd2);
	}
	return 0;
}