/* Projekt: SmartBramex
-- Funkcja: Implementacja serwera - bramy radiowej
-- Ostatnia zmiana: 02/06/2019
-- Autorzy: Piotr Ogrodnik, Krzysztof Kasprzak, Sebastian Kamoda, Mateusz Dudek */

#include <iostream>
#include <fcntl.h>
#include <sys/types.h>

#include "header/server.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: " << argv[0] << " port_number" << endl;
		return(1);
	}
	cout << "IPROTO_UDPLITE: " << IPPROTO_UDPLITE << endl;

	Server *server = new Server(argc, argv);
		
	return 0;
}
