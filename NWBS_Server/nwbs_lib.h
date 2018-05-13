#ifndef __NWBS_LIB_H__
#define __NWBS_LIB_H__

#include "nwbs_types.h"

extern nwbs_dbinfo_t ser_dbinfo;

int socket_init(int *sockfd, const char *ip, unsigned short port);

/*
 * 该函数用来初始化数据库句柄并连接数据库
 */
int sql_init(MYSQL *sql_db);

#endif