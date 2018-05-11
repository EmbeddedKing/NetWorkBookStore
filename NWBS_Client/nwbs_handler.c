#include "nwbs_handler.h"


/*
 * 该函数用来打印帮助手册
 */
void guesthelp_handler()
{
	printf("/----------------------------\n");
	printf("|-----.help:      获取帮助\n");
	printf("|-----.signup:    注册账号\n");
	printf("|-----.signin:    登录账号\n");
	printf("|-----.quit:      退出程序\n");
	printf("|-----.exit:      退出程序\n");
	printf("|----------------------------\n");
}

void userhelp_handler()
{
	printf("/----------------------------\n");
	printf("|-----.help:      获取帮助\n");
	printf("|-----.signup:    注册账号\n");
	printf("|-----.signin:    登录其他账号\n");
	printf("|-----.signout:   注销账号\n");
	printf("|-----.chpswd:    修改账户密码\n");
	printf("|-----.quit:      退出程序\n");
	printf("|-----.exit:      退出程序\n");
	printf("|----------------------------\n");
}

void adminhelp_handler()
{
	printf("/----------------------------\n");
	printf("|-----.help:      获取帮助\n");
	printf("|-----.signout:   注销账号\n");
	printf("|-----.chpswd:    修改账户密码\n");
	printf("|-----.addbook:   添加图书\n");
	printf("|-----.delbook:   删除图书\n");
	printf("|-----.quit:      退出程序\n");
	printf("|-----.exit:      退出程序\n");
	printf("|----------------------------\n");
}

void exit_handler(int sockfd)
{
	exit(0);
}


/* 
 * 该函数用来注册一个账号
 * 该函数会产生3个错误码
 * ERRSEND:发送失败
 * ERRRECV:接收失败
 * ERREXIST:账号已存在
 */
int signup_handler(int sockfd, nwbs_user_t user)
{
	int ret;
	/* 发送接收协议包变量 */
	nwbs_proto_t cli_msg, ser_msg;
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));
	/* 打包协议 */
	cli_msg.proto_opt = CLISIGNUP;
	strcpy(cli_msg.proto_user.username, user.username);
	strcpy(cli_msg.proto_user.useraccount, user.useraccount);
	strcpy(cli_msg.proto_user.userpasswd, user.userpasswd);

	/* 发送协议包 */
	ret = send(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		return -1;
	}

	/* 等待服务器响应 */
	ret = recv(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRRECV);
		return -1;
	}
	else if (ret == 0) {
		nwbs_seterrno(ERRBREAK);
		printf("%s\n", nwbs_error());
		exit(0);
	}

	/* 解析协议包 */
	switch (ser_msg.proto_opt) {
		/* 注册成功 */
		case SERSUCCESS:
			nwbs_seterrno(ERRSUCCESS);
			return 0;
		/* 服务器发生错误 */
		case SERERROR:
			nwbs_seterrno(ERRSERVER);
			return -1;
		/* 账号已存在 */
		case SEREXIST:
			nwbs_seterrno(ERRACCEXIST);
			return -1;
	}
}


/*
 * 该函数用来登录服务器
 */
int signin_handler(int sockfd, nwbs_user_t *user)
{
	int ret;
	nwbs_proto_t cli_msg, ser_msg;
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	cli_msg.proto_opt = CLISIGNIN;
	strcpy(cli_msg.proto_user.useraccount, (*user).useraccount);
	strcpy(cli_msg.proto_user.userpasswd, (*user).userpasswd);
	ret = send(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		return -1;
	}

	ret = recv(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		/* 接收出错 */
		nwbs_seterrno(ERRRECV);
		return -1;
	} else if (ret == 0) {
		/* 与服务器断开连接 */
		nwbs_seterrno(ERRBREAK);
		printf("%s\n", nwbs_error());
		exit(0);
	}

	/* 解析协议包 */
	switch (ser_msg.proto_opt) {
		/* 注册成功 */
		case SERSUCCESS:
			nwbs_seterrno(ERRSUCCESS);
			strcpy((*user).username, ser_msg.proto_user.username);
			return 0;
		/* 服务器发生错误 */
		case SERERROR:
			nwbs_seterrno(ERRSERVER);
			return -1;
		/* 密码不匹配 */
		case SERERRPSWD:
			nwbs_seterrno(ERRPASSWD);
			return -1;
		/* 账号不存在 */
		case SERNOEXIST:
			nwbs_seterrno(ERRACCNOEXIST);
			return -1;
	}
}

int  signout_handler(nwbs_curuser_t *curuser)
{
	memset(curuser, 0, sizeof(nwbs_curuser_t));
	strcpy((*curuser).signinname, "Guest");
}

int chpswd_handler(int sockfd, nwbs_chpswd_t chpswd)
{
	int ret;
	nwbs_proto_t cli_msg, ser_msg;
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	cli_msg.proto_opt = CLICHPSWD;
	strcpy(cli_msg.proto_chpswd.useraccount, chpswd.useraccount);
	strcpy(cli_msg.proto_chpswd.oldpasswd, chpswd.oldpasswd);
	strcpy(cli_msg.proto_chpswd.newpasswd, chpswd.newpasswd);
	ret = send(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		return -1;
	}

	ret = recv(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRRECV);
		return -1;
	} else if (ret == 0) {
		nwbs_seterrno(ERRBREAK);
		printf("%s\n", nwbs_error());
		exit(-1);
	}

	switch(ser_msg.proto_opt) {
		case SERSUCCESS:
			nwbs_seterrno(ERRSUCCESS);
			return 0;
		case SERERROR:
			nwbs_seterrno(ERRSERVER);
			return -1;
		case SERERRPSWD:
			nwbs_seterrno(ERROLDPSWD);
			return -1;
	}
}