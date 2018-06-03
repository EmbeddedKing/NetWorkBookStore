#ifndef __NWBS_TYPES_H__
#define __NWBS_TYPES_H__

/* 所需要的系统头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mysql/mysql.h>
#include <termios.h>
#include <pthread.h>

/* 传送的buf大小 */
#define BUF_SIZE 200
#define __DEBUG_MODE 0

typedef enum {
	SERSUCCESS,
	SERERROR,
	SEREXIST,
	SERNOEXIST,
	SERERRPSWD,
	SERLISTEND,
	SERDOWNEND,

	CLISUCCESS,
	CLIERROR,
	CLISIGNIN,
	CLISIGNUP,
	CLICHPSWD,
	CLIBOOKLIST,
	CLIBOOKDOWN,
	CLIBOOKINFO,
	CLIBOOKUP,
	CLIUPEND,
	CLIBOOKDEL,
	CLIEXIT
}nwbs_opt_t;

/**
 * 该结构体用来储存用户信息
 */
typedef struct _nwbs_user {
	char username[20];
	char useraccount[20];
	char userpasswd[20];
}nwbs_user_t;

typedef struct _nwbs_book {
	int  booknum;
	char bookname[100];
	char bookauthor[50];
	char bookfilename[100];
}nwbs_book_t;

typedef struct _nwbs_chpswd {
	char useraccount[20];
	char oldpasswd[20];
	char newpasswd[20];
}nwbs_chpswd_t;

typedef struct _nwbs_curuser {
	char signinname[20];
	char signinaccount[20];
}nwbs_curuser_t;

typedef struct _nwbs_bookdown {
	int  booknum;
	int  bookdatasize;
	char bookbuf[BUF_SIZE];
}nwbs_bookdown_t;

typedef struct _nwbs_protocol {
	nwbs_opt_t proto_opt;
	nwbs_user_t proto_user;
	nwbs_chpswd_t proto_chpswd;
	nwbs_book_t proto_book;
	nwbs_bookdown_t proto_bookdown;
}nwbs_proto_t;

#endif