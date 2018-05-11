#include "nwbs_types.h"
#include "nwbs_menu.h"
#include "nwbs_errno.h"
#include "nwbs_handler.h"

void com_handler(int sockfd, const char *command);
int socket_connect(int sockfd, const char *ip, unsigned short port);
void echo_off();
void echo_on();

/* 记录当前的登录账户 */
nwbs_curuser_t cli_curuser = {"Guest", ""};

int main(int argc, char const *argv[])
{
	/* 判断传入参数 */
	if (argc != 3) {
		printf("Usage: %s <serip> <serport>\n", argv[0]);
		return -1;
	}

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
		printf("connect error\n");
		return -1;
	}

	int num;
	/* 接收用户的命令 */
	char command[100];

	show_one_prompt();
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

void com_handler(int sockfd, const char *command)
{
	int ret;
	nwbs_user_t cli_user;
	nwbs_chpswd_t cli_chpswd;
	if (strcmp(command, ".help") == 0) {
		if (!strcmp(cli_curuser.signinname, "Guest"))
			guesthelp_handler();
		else if (!strcmp(cli_curuser.signinname, "admin"))
			adminhelp_handler();
		else
			userhelp_handler();

	} else if (strcmp(command, ".signup") == 0) {
		/* 注册操作已完成 */
		memset(&cli_user, 0, sizeof(nwbs_user_t));
		printf("/----------------------------\n");
		printf("|---输入注册昵称：");
		fgets(cli_user.username, sizeof(cli_user.username), stdin);
		cli_user.username[strlen(cli_user.username) - 1] = '\0';
		printf("|---输入注册账号：");
		fgets(cli_user.useraccount, sizeof(cli_user.useraccount), stdin);
		cli_user.useraccount[strlen(cli_user.useraccount) - 1] = '\0';
		printf("|---输入注册密码：");
		fgets(cli_user.userpasswd, sizeof(cli_user.userpasswd), stdin);
		cli_user.userpasswd[strlen(cli_user.userpasswd) - 1] = '\0';
		printf("|----------------------------\n");
		ret = signup_handler(sockfd, cli_user);
		if (nwbs_geterrno())
			printf("%s\n", nwbs_error());
	} else if (strcmp(command, ".signin") == 0) {
		/* 登录操作已完成 */
		memset(&cli_user, 0, sizeof(nwbs_user_t));
		printf("/----------------------------\n");
		printf("|---输入登录账号：");
		fgets(cli_user.useraccount, sizeof(cli_user.useraccount), stdin);
		cli_user.useraccount[strlen(cli_user.useraccount) - 1] = '\0';
		printf("|---输入登录密码：");
		echo_off();
		fgets(cli_user.userpasswd, sizeof(cli_user.userpasswd), stdin);
		cli_user.userpasswd[strlen(cli_user.userpasswd) - 1] = '\0';
		echo_on();
		printf("\n|----------------------------\n");
		signin_handler(sockfd, &cli_user);
		if (!nwbs_geterrno()) {
			strcpy(cli_curuser.signinname, cli_user.username);
			strcpy(cli_curuser.signinaccount, cli_user.useraccount);
		} else
			printf("%s\n", nwbs_error());
	} else if (strcmp(command, ".signout") == 0) {
		/* 注销账号已完成 */
		signout_handler(&cli_curuser);
	} else if (strcmp(command, ".chpswd") == 0) {
		printf("修改账户密码\n");
		if (strcmp(cli_curuser.signinname, "Guest")) {
			strcpy(cli_chpswd.useraccount, cli_curuser.signinaccount);
			printf("/----------------------------\n");
			printf("|---请输入账户旧密码：");
			echo_off();
			fgets(cli_chpswd.oldpasswd, sizeof(cli_chpswd.oldpasswd), stdin);
			cli_chpswd.oldpasswd[strlen(cli_chpswd.oldpasswd) - 1] = '\0';
			printf("\n|---请输入账户新密码：");
			fgets(cli_chpswd.newpasswd, sizeof(cli_chpswd.newpasswd), stdin);
			cli_chpswd.newpasswd[strlen(cli_chpswd.newpasswd) - 1] = '\0';
			echo_on();
			printf("\n|----------------------------\n");
			chpswd_handler(sockfd, cli_chpswd);
			if (nwbs_geterrno()) {
				printf("%s\n", nwbs_error());
			} else {
				printf("密码已重置，请重新登录\n");
				signout_handler(&cli_curuser);
			}
		} else {
			printf("当前以游客身份登录，请登录账号\n");
		}
	} else if ((strcmp(command, ".exit") == 0) || (strcmp(command, ".quit") == 0)) {
		/* 退出已完成 */
		printf("感谢使用网上书店客户端，欢迎下次使用\n");
		exit_handler();
	} else {
		printf("无法识别的命令，请重新输入\n");
	}
}

int socket_connect(int sockfd, const char *ip, unsigned short port)
{
	struct sockaddr_in ser_addr;
	memset(&ser_addr, 0, sizeof(struct sockaddr_in));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = inet_addr(ip);
	socklen_t ser_addrlen = sizeof(struct sockaddr_in);
	int ret;
	/* 向服务器发送连接请求 */
	ret = connect(sockfd, (struct sockaddr *)&ser_addr, ser_addrlen);
	if (ret < 0) {
		perror("connect");
		return -1;
	}
	return 0;
}

void echo_off()
{
	struct termios oldtermios, newtermios;
	tcgetattr(STDIN_FILENO, &oldtermios);
	newtermios = oldtermios;
	newtermios.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO,TCSANOW,&newtermios);
}

void echo_on()
{
	struct termios oldtermios, newtermios;
	tcgetattr(STDIN_FILENO, &oldtermios);
	newtermios = oldtermios;
	newtermios.c_lflag |= ECHO | ICANON;
	tcsetattr(STDIN_FILENO,TCSANOW,&newtermios);
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
