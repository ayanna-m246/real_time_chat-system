#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<string.h>
#include<malloc.h>

using namespace std;

#pragma once

int createTCPIpv4Socket()
{
    return socket(AF_INET, SOCK_STREAM, 0);
}
struct sockaddr_in* createSocketAddress(const char* ip, int port) {
    struct sockaddr_in *address=new struct sockaddr_in(); ;
    address->sin_family = AF_INET;
    address->sin_port = htons(port);

    if(strlen(ip)==0){
        address->sin_addr.s_addr=INADDR_ANY;
    }
    else{
        inet_pton(AF_INET, ip, &address->sin_addr.s_addr);
    }
    return address;
}

