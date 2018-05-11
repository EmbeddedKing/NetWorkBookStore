#ifndef __NWBS_TYPES_H__
#define __NWBS_TYPES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>
#include <mysql/mysql.h>

#define BUF_SIZE 200

typedef enum {
	SERSUCCESS,
	SERERROR,
	SEREXIST,
	SERNOEXIST,
	SERERRPSWD,
	CLISUCCESS,
	CLISIGNIN,
	CLISIGNUP,
	CLICHPSWD,
	CLIEXIT
}nwbs_opt_t;

typedef struct _nwbs_user {
	char username[20];
	char useraccount[20];
	char userpasswd[20];
}nwbs_user_t;

typedef struct _nwbs_chpswd {
	char useraccount[20];
	char oldpasswd[20];
	char newpasswd[20];
}nwbs_chpswd_t;

typedef struct _nwbs_protocol {
	nwbs_opt_t proto_opt;
	nwbs_user_t proto_user;
	nwbs_chpswd_t proto_chpswd;
}nwbs_proto_t;

#endif