/* Projekt: SmartBramex
-- Funkcja: Implementacja serwera - bramy radiowej
-- Ostatnia zmiana: 01/06/2019
-- Autorzy: Piotr Ogrodnik, Krzysztof Kasprzak, Sebastian Kamoda */

#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
/*#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unordered_map>
#include <vector>
#include <list>

#include "header/socket_helper.h"*/

#include "header/server.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: %s port_number\n" << argv[0] << endl;
		return(1);
	}
	cout << "IPROTO_UDPLITE: " << IPPROTO_UDPLITE << endl;

	Server *server = new Server(argc, argv);
		
	return 0;
}
