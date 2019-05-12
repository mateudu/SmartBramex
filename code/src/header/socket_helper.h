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

#define UDPLITE_SEND_CSCOV  10
#define UDPLITE_RECV_CSCOV  11

using namespace std;

#define MAX_BUF 1024

enum Status {inactive, active};

struct addr_info{
    struct sockaddr_in* addr_info;
    int fd;
};

struct checksum{
    int clientID;
    size_t messageID;
    Status statusID;
};

struct addr_info* createUdpLiteSocket(int port, char *address = nullptr);

void set_checksum_on_socket(int sock_fd, size_t n_o_bytes, short int type);

void set_timeout_on_socket(int sock_fd, time_t n_o_seconds);

void generate_message(char buffer[], struct checksum& metadata, string content);

void get_message_metadata(checksum& checksum_struct, char* buffer);

string get_message_content(char* buffer);