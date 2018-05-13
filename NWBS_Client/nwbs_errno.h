#ifndef __NWBS_ERRNO_H__
#define __NWBS_ERRNO_H__

#include "nwbs_types.h"

/* errno level */
#define ERRFATAL       0
#define ERRWARRING     1

/* errno define */
#define ERRSUCCESS     0
#define ERRSEND        1
#define ERRRECV        2
#define ERRSERVER      3
#define ERRBREAK       4
#define ERRACCEXIST    5
#define ERRACCNOEXIST  6
#define ERRPASSWD      7
#define ERROLDPSWD     8
#define ERRFILEOPT     9
#define ERRBKNOEXIST   10

typedef struct _nwbs_errhandler {
	void (*errsuccess_handler)(int errorno);
	void (*errsend_handler)(int errorno);
	void (*errrecv_handler)(int errorno);
	void (*errserver_handler)(int errorno);
	void (*errbreak_handler)(int errorno);
	void (*erraccexist_handler)(int errorno);
	void (*erraccnoexist_handler)(int errorno);
	void (*errpasswd_handler)(int errorno);
	void (*erroldpswd_handler)(int errorno);
}nwbs_errhandler_t;

int nwbs_errno;
int errorlevel;
extern nwbs_errhandler_t nwbs_errhandler;

/* 用来返回错误码 */
int nwbs_geterrno();
/* 用来设置错误码 */
int nwbs_seterrno(int errorno);
/* 用来返回错误信息 */
char* nwbs_error();
/* 默认处理函数 */
void nwbs_errdefaulthandler(int errorno);
/* 改变处理函数 */
int nwbs_cherrhandler(int errorno, void (*handler)(int));
/* 还原默认函数 */
int nwbs_reerrhandler(int errorno);
/* 执行错误处理函数 */
int nwbs_exechandler(int errorno);

#endif