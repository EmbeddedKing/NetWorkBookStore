#ifndef __NWBS_HANDLER_H__
#define __NWBS_HANDLER_H__

#include "nwbs_types.h"
#include "nwbs_errno.h"
#include "nwbs_lib.h"

/* 处理客户端协议的函数 */
int msg_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理成功的函数 */
int success_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理用户注册的函数 */
int signup_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理用户登录的函数 */
int signin_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理用户修改密码的函数 */
int chpswd_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理用户获取图书列表的函数 */
int booklist_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理用户下载图书的函数 */
int bookdown_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理获取图书信息的函数 */
int bookinfo_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理管理员上传图书的函数 */
int bookup_handler(int sockfd, nwbs_proto_t cli_msg);
/* 处理客户端协议的函数 */
int exit_handler();

#endif