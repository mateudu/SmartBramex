#include "header/socket_helper.h"

struct addr_info* createUdpLiteSocket(int port, char *address){
    struct addr_info* socket_addr_info = new addr_info;
    socket_addr_info->addr_info = new sockaddr_in;

    // Creating message file descriptor 
    socket_addr_info->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE);
	if (socket_addr_info->fd == -1){
		throw "Socket creation unsuccessful";
	}
  
    //memset(&(socket_addr_info->addr_info), 0, sizeof(socket_addr_info->addr_info));
    
    socket_addr_info->addr_info->sin_family = AF_INET;
	socket_addr_info->addr_info->sin_port = htons(port);
    if (address != nullptr)
    {
        inet_aton(address, &(socket_addr_info->addr_info->sin_addr));
    } else {
        socket_addr_info->addr_info->sin_addr.s_addr = INADDR_ANY;
    }

    return socket_addr_info;
}