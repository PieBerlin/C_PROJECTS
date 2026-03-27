// cache22.h

#ifndef CACHE22_H
#define CACHE22_H
#define _GNU_SOURCE

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<arpa/inet.h>
#include<assert.h>
#include<errno.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include <stddef.h>
#include<stdarg.h>


#pragma GCC diagnostic ignored "-Wimplicit-int"
#pragma GCC diagnostic push



#define HOST "127.0.0.1"
#define PORT "12049"



typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;

struct s_client{
    int s;

    char ip[16];
    int16 port;
};
typedef struct s_client Client;

// functions
void zero(int8*, int16);
void childloop(Client*);
void mainloop(int);
int initserver(int16);
int main(int, char **);


#endif