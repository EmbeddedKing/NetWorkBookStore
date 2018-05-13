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
#include <pthread.h>

#define BUF_SIZE         200

/* book_info表字段的顺序 */
#define DBBOOKNUM        0
#define DBBOOKNAME       1
#define DBBOOKAUTHOR     2
#define DBBOOKFILENAME   3
#define DBBOOKPATH       4

/* user_info表字段的顺序 */
#define DBUSERNAME       0
#define DBUSERACCOUNT    1
#define DBUSERPASSWD     2

/* 该枚举类型用来指定用户协议操作 */
typedef enum {
	SERSUCCESS,
	SERERROR,
	SEREXIST,
	SERNOEXIST,
	SERERRPSWD,
	SERLISTEND,
	SERDOWNEND,
	CLISUCCESS,
	CLISIGNIN,
	CLISIGNUP,
	CLICHPSWD,
	CLIBOOKLIST,
	CLIBOOKDOWN,
	CLIBOOKINFO,
	CLIBOOKUP,
	CLIEXIT
}nwbs_opt_t;

/* 该结构体用来装数据库信息 */
typedef struct _nwbs_dbinfo {
	char dbhost[50];
	char dbuser[50];
	char dbpasswd[50];
	int  dbport;
	char dbname[50];
} nwbs_dbinfo_t;

/* 该结构体用来装用户信息 */
typedef struct _nwbs_user {
	char username[20];
	char useraccount[20];
	char userpasswd[20];
}nwbs_user_t;

/* 该结构体用来装图书信息 */
typedef struct _nwbs_book {
	int booknum;
	char bookname[100];
	char bookauthor[50];
	char bookfilename[100];
}nwbs_book_t;

/* 该结构体用来装修改密码信息 */
typedef struct _nwbs_chpswd {
	char useraccount[20];
	char oldpasswd[20];
	char newpasswd[20];
}nwbs_chpswd_t;

typedef struct _nwbs_bookdown {
	int booknum;
	int bookdatasize;
	char bookbuf[BUF_SIZE];
}nwbs_bookdown_t;

/* 该结构体用来传输协议 */
typedef struct _nwbs_protocol {
	nwbs_opt_t proto_opt;
	nwbs_user_t proto_user;
	nwbs_chpswd_t proto_chpswd;
	nwbs_book_t proto_book;
	nwbs_bookdown_t proto_bookdown;
}nwbs_proto_t;

#endif