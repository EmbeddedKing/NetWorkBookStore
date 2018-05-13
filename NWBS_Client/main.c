#include "nwbs_types.h"
#include "nwbs_menu.h"
#include "nwbs_errno.h"
#include "nwbs_handler.h"
#include "nwbs_lib.h"

/**
 * 记录当前登录账户
 */
nwbs_curuser_t cli_curuser = {"Guest", ""};
void *thread_chackerror(void *arg);

int main(int argc, char const *argv[])
{
    /* 判断传入参数 */
    if (argc != 3) {
		printf("Usage: %s <serip> <serport>\n", argv[0]);
		return -1;
	}
	//system("clear");
	set_backspace();
	/* 客户端套接字 */
	int cli_sockfd;
	/* 初始化套接字 */
	cli_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cli_sockfd < 0) {
		perror("cli_sockfd socket");
		return -1;
	}

	/* 发送连接请求 */
	int ret;
	ret = socket_connect(cli_sockfd, argv[1], atoi(argv[2]));
	if (ret < 0) {
		printf("无法连接服务器\n");
		return -1;
	}

	struct timeval nwbs_timeout = {
		.tv_sec  = 5,
		.tv_usec = 0
	};

	int num;
	/* 接收用户的命令 */
	char command[100];
	show_one_prompt();
	/* 开启错误检测线程 */
	pthread_t check_thread;
	pthread_create(&check_thread, NULL, thread_chackerror, NULL);
	while (1) {
		printf("%s>", cli_curuser.signinname);
		bzero(command, sizeof(command));
		/* 输入命令 */
		fgets(command, sizeof(command), stdin);
		command[strlen(command) - 1] = '\0';
		/* 处理命令 */
		com_handler(cli_sockfd, command);
	}

	return 0;
}

void *thread_chackerror(void *arg)
{
	while (1) {
		if (nwbs_geterrno()) {
			nwbs_exechandler(nwbs_geterrno());
			nwbs_seterrno(ERRSUCCESS);
		}
	}
}
