#ifndef __NWBS_HANDLER_H__
#define __NWBS_HANDLER_H__

#include "nwbs_types.h"
#include "nwbs_errno.h"
#include "nwbs_lib.h"

extern nwbs_curuser_t cli_curuser;

/* 用户输入命令解析 */
void com_handler(int sockfd, const char *command);
/* 游客支持的命令 */
void guesthelp_handler();
/* 管理员支持的命令 */
void adminhelp_handler();
/* 普通用户支持的命令 */
void userhelp_handler();
/* 退出处理 */
void exit_handler();
/* 注册处理 */
int  signup_handler(int sockfd, nwbs_user_t user);
/* 登录处理 */
int  signin_handler(int sockfd, nwbs_user_t *user);
/* 注销处理 */
int  signout_handler(nwbs_curuser_t *curuser);
/* 修改密码处理 */
int  chpswd_handler(int sockfd, nwbs_chpswd_t chpswd);
/* 获取图书列表处理 */
int  booklist_handler(int sockfd);
/* 下载图书处理 */
int  bookdown_handler(int sockfd, int booknum, const char *bookpath);
/* 上传图书处理 */
int  bookup_handler(int sockfd);
/* 得到数据结构体 */
int  bookinfo_handler(int sockfd, nwbs_book_t *book);

#endif