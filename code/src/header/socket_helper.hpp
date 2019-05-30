#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <sys/socket.h>
#include <list>

#define UDPLITE_SEND_CSCOV  10
#define UDPLITE_RECV_CSCOV  11

using namespace std;

#define MAX_BUF 1024

enum status {status_inactive, status_active};
enum message_type {message_type_data, message_type_data_request, message_type_data_request_response};

struct addr_info{
    struct sockaddr_in* addr_info;
    int fd;
};

struct metadata{
    int client_id;
    size_t message_id;
    status status_id;
    message_type message_type_id;
};

struct client_info{
	list<size_t> missing_messages_id;
	size_t expected_message_id;
};

struct addr_info* createUdpLiteSocket(int port, char *address = nullptr);

void set_checksum_on_socket(int sock_fd, size_t n_o_bytes, short int type);

void set_timeout_on_socket(int sock_fd, time_t n_o_seconds);

void generate_message(char buffer[], struct metadata& metadata, string content);

void get_message_metadata(struct metadata& metadata_struct, char* buffer);

string get_message_content(char* buffer);

