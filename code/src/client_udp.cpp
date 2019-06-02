/* Projekt: SmartBramex
-- Funkcja: Logika dla klienta - czujnika
-- Ostatnia zmiana: 02/06/2019
-- Autorzy: Piotr Ogrodnik, Krzysztof Kasprzak, Sebastian Kamoda, Mateusz Dudek */

#include "header/client_udp.hpp"

using namespace std;

int main(int argc, char* argv[]) 
{ 
    if (argc < 7)	{
		cout << "Usage: " << argv[0] << " ip_address port_number client_number interval jitter start_message_id" << endl;
		return(1);
	}

    Client_udp *client = new Client_udp(argc, argv);

    return 0; 
} 
