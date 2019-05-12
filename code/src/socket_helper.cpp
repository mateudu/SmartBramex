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

void set_checksum_on_socket(int sock_fd, size_t n_o_bytes, short int type){
    setsockopt(sock_fd, SOL_SOCKET, type, &n_o_bytes, sizeof(n_o_bytes));
}

void set_timeout_on_socket(int sock_fd, time_t n_o_seconds){
    struct timeval tv_struct;
    tv_struct.tv_sec = n_o_seconds;
    tv_struct.tv_usec = 0;

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_struct, sizeof(tv_struct));
}

void generate_message(char buffer[], struct checksum& metadata, string content){
    //make sure the messaage is not too big
    if ((sizeof(struct checksum) + content.length()) >= MAX_BUF)
        throw "MESSAGE SIZE IS BIGGER THAN MAX_BUF. INCRASE MAX_BUF";
    
    //copy metadata, which will be protected
    size_t offset = 0;
    memcpy(buffer + offset, &metadata, sizeof(struct checksum));
    offset += sizeof(struct checksum);

    //copy the rest of the message
    memcpy(buffer + offset, content.c_str(), content.length());
}

void get_message_metadata(checksum& checksum_struct, char* buffer){
    //Copy the struct that we received to the struct passed into argument
    memcpy(&checksum_struct, buffer, sizeof(struct checksum));
}

string get_message_content(char* buffer){
    //Return content that's not checksum from received message as a std::string
    char temp_buff[MAX_BUF];
    memcpy(temp_buff, buffer + sizeof(struct checksum), MAX_BUF - sizeof(struct checksum));
    string str(temp_buff);
    return str;
}