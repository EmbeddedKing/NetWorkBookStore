#ifndef __NWBS_ERRNO_H__
#define __NWBS_ERRNO_H__

#include "nwbs_types.h"

/* errno level */
#define ERRFATAL       0
#define ERRWARRING     1

/* errno define */
#define ERRSUCCESS     0  /* 成功 */
#define ERRSQLINIT     1  /* 数据库初始化失败 */
#define ERRSQLQUERY    2  /* 数据库命令执行失败 */
#define ERRSEND        3  /* 发送失败 */
#define ERRRECV        4  /* 接收失败 */
#define ERRFILEOPT     5  /* 文件操作失败 */
#define ERRCLIENT      6  /* 客户端出现异常 */

/* 错误码对应的错误函数 */
typedef struct _nwbs_errhandler {
	void (*errsuccess_handler)(int errorno);
	void (*errsend_handler)(int errorno);
	void (*errsqlinit_handler)(int errorno);
	void (*errsqlquery_handler)(int errorno);
}nwbs_errhandler_t;

int nwbs_errno;
int nwbs_errlevel;
extern nwbs_errhandler_t nwbs_errhandler;

/* 用来返回错误码 */
int nwbs_geterrno();
/* 用来设置错误码 */
int nwbs_seterrno(int errorno);
/* 用来返回错误信息 */
char *nwbs_error();
/* 默认处理函数 */
void nwbs_errdefaulthandler(int errorno);
/* 改变处理函数 */
int nwbs_cherrhandler(int errorno, void (*handler)(int));
/* 还原默认函数 */
int nwbs_reerrhandler(int errorno);
/* 执行错误处理函数 */
int nwbs_exechandler(int errorno);


#endif