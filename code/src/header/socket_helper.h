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

using namespace std;

#define MAX_BUF 1024

struct addr_info{
    struct sockaddr_in* addr_info;
    int fd;
};

struct chcksum{
    string clientID;
    int messageID;
};

struct addr_info* createUdpLiteSocket(int port, char *address = nullptr, struct chcksum checksum);