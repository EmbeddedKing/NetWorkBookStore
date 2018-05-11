#include "nwbs_types.h"
#include "nwbs_handler.h"
#include "nwbs_errno.h"

int socket_init(int *sockfd, const char *ip, unsigned short port);

int main(int argc, char const *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <serip> <serport>\n", argv[0]);
		return -1;
	}

	/* lis_sockfd 用来监听端口的监听套接字 */
	int lis_sockfd;
	/* 接收返回值 */
	int ret;

	/* 初始化套接字 */
	ret = socket_init(&lis_sockfd, argv[1], atoi(argv[2]));
	if (ret < 0) {
		printf("socket_init error\n");
		return -1;
	}

	/* 接收和发送的字节数 */
	int num;
	/* 子进程号 */
	pid_t branch;
	/* con_sockfd 用来处理连接的连接套接字 */
	int con_sockfd;
	/* con_addr 连接的客户端地址 */
	struct sockaddr_in con_addr;
	memset(&con_addr, 0, sizeof(struct sockaddr_in));
	socklen_t con_addrlen = sizeof(struct sockaddr_in);
	
	/* 用来接收客户端传过来的数据 */
	nwbs_proto_t cli_msg, ser_msg;
	while (1)
	{
		/* 处理连接请求 */
		con_sockfd = accept(lis_sockfd, (struct sockaddr *)&con_addr, &con_addrlen);
		if (con_sockfd < 0) {
			perror("accept");
			return -1;
		}

		/* 分支 */
		branch = fork();
		if (branch < 0) {
			perror("fork");
			return -1;
		} else if (branch == 0) {
			/* 子进程处理客户端请求 */
			close(lis_sockfd);
			while (1)
			{
				memset(&cli_msg, 0, sizeof(nwbs_proto_t));
				num = recv(con_sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
				if (num < 0)
				{
					perror("recv");
					return -1;
				}
				else if(num == 0)
				{
					return 0;
				}
				/* 传入套接字, 处理客户端发过来的消息 */
				msg_handler(con_sockfd, cli_msg);
			}
		} else {
			/* 父进程继续监听端口 */
			close(con_sockfd);
		}
	}
	return 0;
}

int socket_init(int *sockfd, const char *ip, unsigned short port)
{
	/* 初始化套接字 */
	(*sockfd) = socket(AF_INET, SOCK_STREAM, 0);
	if ((*sockfd) < 0) {
		perror("lis_sockfd socket");
		return -1;
	}

	/* ser_addr 服务器的IP地址 */
	struct sockaddr_in ser_addr;
	memset(&ser_addr, 0, sizeof(struct sockaddr_in));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = inet_addr(ip);
	
	/* ser_addrlen 服务器地址的长度 */
	socklen_t ser_addrlen = sizeof(struct sockaddr_in);
	int ret;
	/* 绑定端口 */
	ret = bind((*sockfd), (struct sockaddr *)&ser_addr, ser_addrlen);
	if (ret < 0) {
		perror("bind");
		return -1;
	}

	/* 开启端口监听 */
	ret = listen((*sockfd), 20);
	if (ret < 0) {
		perror("listen");
		return -1;
	}
}

void *thread_checkerror(void *arg)
{
	if (nwbs_geterrno()) {
		nwbs_exechandler(nwbs_geterrno());
		nwbs_seterrno(ERRSUCCESS);
	}
}