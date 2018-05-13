#include "nwbs_errno.h"

nwbs_errhandler_t nwbs_errhandler = {
	.errsuccess_handler    = nwbs_errdefaulthandler,
	.errsend_handler       = nwbs_errdefaulthandler,
	.errsqlinit_handler    = nwbs_errdefaulthandler,
	.errsqlquery_handler   = nwbs_errdefaulthandler
};

int nwbs_geterrno()
{
	return nwbs_errno;
}

int nwbs_seterrno(int errorno)
{
	nwbs_errno = errorno;
}

char* nwbs_error()
{
	switch (nwbs_geterrno()) {
		case ERRSUCCESS:
			return "成功\n";
		case ERRSQLINIT:
			return "数据库初始化失败\n";
		case ERRSQLQUERY:
			return "数据库命令执行失败\n";
		case ERRSEND:
			return "发送协议包失败\n";
	}
}

void nwbs_errdefaulthandler(int errorno)
{
	switch (errorno) {
		case ERRSUCCESS:
		case ERRSQLINIT:
		case ERRSQLQUERY:
		case ERRSEND:
			printf("error default handler\n");
			break;
		default:
			printf("不知道的错误类型\n");
			break;
	}
}

int nwbs_cherrhandler(int errorno, void (*handler)(int))
{
	if (handler < 0) {
		return -1;
	}
	switch (errorno) {
		case ERRSUCCESS:
			nwbs_errhandler.errsuccess_handler = handler;
			return 0;
		case ERRSQLINIT:
			nwbs_errhandler.errsqlinit_handler = handler;
			return 0;
		case ERRSQLQUERY:
			nwbs_errhandler.errsqlquery_handler = handler;
			return 0;
		case ERRSEND:
			nwbs_errhandler.errsend_handler = handler;
			return 0;
		default:
			return -1;
	}
}

int nwbs_reerrhandler(int errorno)
{
	switch (errorno) {
		case ERRSUCCESS:
			nwbs_errhandler.errsuccess_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRSQLINIT:
			nwbs_errhandler.errsqlinit_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRSQLQUERY:
			nwbs_errhandler.errsqlquery_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRSEND:
			nwbs_errhandler.errsend_handler = nwbs_errdefaulthandler;
			return 0;
		default:
			return -1;
	}
}
/* 执行错误处理函数 */
int nwbs_exechandler(int errorno)
{
	switch (errorno) {
		case ERRSUCCESS:
			nwbs_errhandler.errsuccess_handler(errorno);
			return 0;
		case ERRSQLINIT:
			nwbs_errhandler.errsqlinit_handler(errorno);
			return 0;
		case ERRSQLQUERY:
			nwbs_errhandler.errsqlquery_handler(errorno);
			return 0;
		case ERRSEND:
			nwbs_errhandler.errsend_handler(errorno);
			return 0;
		default:
			return -1;
	}
}