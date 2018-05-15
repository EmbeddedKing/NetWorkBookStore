#include "nwbs_errno.h"

nwbs_errhandler_t nwbs_errhandler = {
	.errsuccess_handler    = nwbs_errdefaulthandler,
	.errsend_handler       = nwbs_errdefaulthandler,
	.errrecv_handler       = nwbs_errdefaulthandler,
	.errserver_handler     = nwbs_errdefaulthandler,
	.errbreak_handler      = nwbs_errdefaulthandler,
	.erraccexist_handler   = nwbs_errdefaulthandler,
	.erraccnoexist_handler = nwbs_errdefaulthandler,
	.errpasswd_handler     = nwbs_errdefaulthandler,
	.erroldpswd_handler    = nwbs_errdefaulthandler,
	.errbkexist_handler    = nwbs_errdefaulthandler,
	.errbknoexist_handler  = nwbs_errdefaulthandler,
	.errfileopt_handler    = nwbs_errdefaulthandler
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
		case ERRSERVER:
			return "服务器发生错误，请联系工作人员\n";
		case ERRSEND:
			return "发送协议包失败\n";
		case ERRRECV:
			return "接收协议包失败\n";
		case ERRBREAK:
			return "与服务器断开连接,请重新启动客户端\n";
		case ERRACCEXIST:
			return "账号已存在\n";
		case ERRPASSWD:
			return "密码错误\n";
		case ERROLDPSWD:
			return "旧密码错误\n";
		case ERRACCNOEXIST:
			return "账号不存在\n";
		case ERRBKEXIST:
			return "书籍已存在\n";
		case ERRBKNOEXIST:
			return "书籍不存在\n";
		case ERRFILEOPT:
			return "文件操作失败\n";
	}
}

void nwbs_errdefaulthandler(int errorno)
{
	switch (errorno) {
		case ERRSUCCESS:
			printf("%s", nwbs_error());
			break;
		case ERRSERVER:
			printf("%s", nwbs_error());
			break;
		case ERRSEND:
			printf("%s", nwbs_error());
			break;
		case ERRRECV:
			printf("%s", nwbs_error());
			break;
		case ERRBREAK:
			printf("%s", nwbs_error());
			break;
		case ERRPASSWD:
			printf("%s", nwbs_error());
			break;
		case ERROLDPSWD:
			printf("%s", nwbs_error());
			break;
		case ERRACCEXIST:
			printf("%s", nwbs_error());
			break;
		case ERRACCNOEXIST:
			printf("%s", nwbs_error());
			break;
		case ERRBKEXIST:
			printf("%s", nwbs_error());
			break;
		case ERRFILEOPT:
			printf("%s", nwbs_error());
			break;
		default:
			printf("不知道的错误类型");
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
		case ERRSERVER:
			nwbs_errhandler.errserver_handler = handler;
			return 0;
		case ERRSEND:
			nwbs_errhandler.errsend_handler = handler;
			return 0;
		case ERRRECV:
			nwbs_errhandler.errrecv_handler = handler;
			return 0;
		case ERRBREAK:
			nwbs_errhandler.errbreak_handler = handler;
			return 0;
		case ERRACCEXIST:
			nwbs_errhandler.erraccexist_handler = handler;
			return 0;
		case ERRPASSWD:
			nwbs_errhandler.errpasswd_handler = handler;
			return 0;
		case ERROLDPSWD:
			nwbs_errhandler.erroldpswd_handler = handler;
			return 0;
		case ERRACCNOEXIST:
			nwbs_errhandler.erraccnoexist_handler = handler;
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
		case ERRSERVER:
			nwbs_errhandler.errserver_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRSEND:
			nwbs_errhandler.errsend_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRRECV:
			nwbs_errhandler.errrecv_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRBREAK:
			nwbs_errhandler.errbreak_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRACCEXIST:
			nwbs_errhandler.erraccexist_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRPASSWD:
			nwbs_errhandler.errpasswd_handler = nwbs_errdefaulthandler;
			return 0;
		case ERROLDPSWD:
			nwbs_errhandler.erroldpswd_handler = nwbs_errdefaulthandler;
			return 0;
		case ERRACCNOEXIST:
			nwbs_errhandler.erraccnoexist_handler = nwbs_errdefaulthandler;
			return 0;
		default:
			return -1;
	}
}

int nwbs_exechandler(int errorno)
{
	switch (errorno) {
		case ERRSUCCESS:
			nwbs_errhandler.errsuccess_handler(errorno);
			return 0;
		case ERRSERVER:
			nwbs_errhandler.errserver_handler(errorno);
			return 0;
		case ERRSEND:
			nwbs_errhandler.errsend_handler(errorno);
			return 0;
		case ERRRECV:
			nwbs_errhandler.errrecv_handler(errorno);
			return 0;
		case ERRBREAK:
			nwbs_errhandler.errbreak_handler(errorno);
			exit(-1);
		case ERRPASSWD:
			nwbs_errhandler.errpasswd_handler(errorno);
			return 0;
		case ERROLDPSWD:
			nwbs_errhandler.erroldpswd_handler(errorno);
			return 0;
		case ERRACCEXIST:
			nwbs_errhandler.erraccexist_handler(errorno);
			return 0;
		case ERRACCNOEXIST:
			nwbs_errhandler.erraccnoexist_handler(errorno);
			return 0;
		case ERRBKEXIST:
			nwbs_errhandler.errbkexist_handler(errorno);
			return 0;
		case ERRBKNOEXIST:
			nwbs_errhandler.errbknoexist_handler(errorno);
			return 0;
		case ERRFILEOPT:
			nwbs_errhandler.errfileopt_handler(errorno);
			return 0;

		default:
			return -1;
	}
}
