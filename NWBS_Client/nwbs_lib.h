#ifndef __NWBS_LIB_H__
#define __NWBS_LIB_H__

#include "nwbs_types.h"

/**
 * 这个函数用来初始化socket
 * @param  sockfd 套接字
 * @param  ip     服务器地址
 * @param  port   服务器端口
 * @return        成功返回0，失败返回-1
 */
int  socket_connect(int sockfd, const char *ip, unsigned short port);

/**
 * 该函数用来使能回退字符
 */
void set_backspace();

/**
 * 该函数用来开启回显输入，一般用在输入密码
 * 下面那个函数用来关闭回显输入
 */
void echo_on();
void echo_off();

/**
 * 该函数用来打印图书信息
 * @param book book结构体，用来描述一本书
 */
void display_bookinfo(nwbs_book_t book);

#endif