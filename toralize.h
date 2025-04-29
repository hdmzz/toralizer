#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#define PROXY "127.0.0.1"
#define PROXYPORT 9050 //le port 9050 ne fonctionnera pas sur toutes les machines mais le 80 (HTTP) oui
#define reqsize sizeof(struct proxy_request)
#define ressize sizeof(struct proxy_response)
#define USERNAME "toraliz"

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;

/**
 * Struct de la proxy REQUEST
 * 		+----+----+----+----+----+----+----+----+----+----+....+----+
		| VN | CD | DSTPORT |      DSTIP        | USERID       |NULL|
		+----+----+----+----+----+----+----+----+----+----+....+----+
    	   1    1      2              4           variable       1      ======== nombres de bytes
*/

struct proxy_request
{
    int8   vn;
    int8   cd;
    int16  dstport;
    int32  dstip;
    char   userid[8];
};

typedef struct proxy_request Req;//comme dans le js

/**
  * Struct de la reponse du proxy
  * 	+----+----+----+----+----+----+----+----+
		| VN | CD | DSTPORT |      DSTIP        |
		+----+----+----+----+----+----+----+----+
     	   1    1      2              4                 ==== nombre de bytes
*/


struct proxy_response
{
    int8    vn;
    int8    cd;
    int16   dstport;
    int32   dstip;
};

typedef struct proxy_response Res;

Req *request(struct sockaddr_in *);
int connect(int, const struct sockaddr *, socklen_t);
