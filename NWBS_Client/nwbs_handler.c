#include "nwbs_handler.h"

/*
 * 该函数用来处理用户输入的命令
 */
void com_handler(int sockfd, const char *command)
{
	int ret;
	int cli_booknum;
	char cli_bookpath[200];
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
		/* 如果出错等待错误处理函数执行 */
		while (nwbs_geterrno() != ERRSUCCESS);
		if (ret == 0) {
			printf("注册成功\n");
		}
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
		ret = signin_handler(sockfd, &cli_user);
		/* 等待错误函数处理完成 */
		while (nwbs_geterrno() != ERRSUCCESS);
		/* 用返回值来判断是否出错 */
		if (ret == 0) {
			strcpy(cli_curuser.signinname, cli_user.username);
			strcpy(cli_curuser.signinaccount, cli_user.useraccount);
			printf("登录成功，欢迎您，%s\n", cli_curuser.signinname);
		}
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
			ret = chpswd_handler(sockfd, cli_chpswd);
			while (nwbs_geterrno() != SERSUCCESS);
			if (ret == 0) {
				signout_handler(&cli_curuser);
				printf("密码已重置，请重新登录\n");
			}
		} else {
			printf("当前为游客身份，请登录账号\n");
		}
	} else if (strcmp(command, ".booklist") == 0) {
		/* 获取图书表 */
		if (strcmp(cli_curuser.signinname, "Guest")) {
			ret = booklist_handler(sockfd);
			while (nwbs_geterrno() != SERSUCCESS);
			if (ret == 0) {
				printf("获取图书列表成功\n");
			} else if (ret < 0) {
				printf("获取图书列表失败\n");
			}
		} else {
			printf("当前为游客身份，请登录账号\n");
		}
	} else if (strcmp(command, ".bookdown") == 0) {
		if (strcmp(cli_curuser.signinname, "Guest")) {
			bzero(cli_bookpath, sizeof(cli_bookpath));
			printf("/----------------------------\n");
			printf("|---请输入要下载的图书编号：");
			scanf("%d", &cli_booknum);
			getchar();
			printf("|---请输入要保存的路径：");
			fgets(cli_bookpath, sizeof(cli_bookpath), stdin);
			cli_bookpath[strlen(cli_bookpath) - 1] = '\0';
			printf("|----------------------------\n");
			ret = bookdown_handler(sockfd, cli_booknum, cli_bookpath);
			while (nwbs_geterrno() != SERSUCCESS);
			if (ret == 0) {
				printf("下载图书成功\n");
			} else if (ret < 0) {
				printf("下载图书失败\n");
			}
		} else {
			printf("当前为游客身份，请登录账号\n");
		}
	} else if (strcmp(command, ".bookup") == 0) {
		bookup_handler(sockfd);
	} else if (strcmp(command, ".bookinfo") == 0) {
		if (strcmp(cli_curuser.signinname, "Guest")) {
			nwbs_book_t book;
			memset(&book, 0, sizeof(nwbs_book_t));
			printf("/----------------------------\n");
			printf("|---请输入要查询的图书编号：");
			scanf("%d", &book.booknum);
			getchar();
			printf("|----------------------------\n");
			ret = bookinfo_handler(sockfd, &book);
			while (nwbs_geterrno() != ERRSUCCESS);
			if (!ret) {
				printf("/-------------------------------------------------------------------------------|\n");
				printf("|---booknum-----bookname----------------bookauthor--------------bookfilename----|\n");
				display_bookinfo(book);
				printf("|-------------------------------------------------------------------------------|\n");
				printf("查询成功\n");
			}
		} else {
			printf("当前为游客身份，请登录账号\n");
		}
	} else if ((strcmp(command, ".exit") == 0) || (strcmp(command, ".quit") == 0)) {
		/* 退出已完成 */
		printf("感谢使用网上书店客户端，欢迎下次使用\n");
		exit_handler();
	} else {
		printf("无法识别的命令，请重新输入\n");
	}
}

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
	printf("|-----.booklist:  获取图书列表\n");
	printf("|-----.bookdown:  下载图书\n");
	printf("|-----.bookinfo:  获取图书信息\n");
	printf("|-----.quit:      退出程序\n");
	printf("|-----.exit:      退出程序\n");
	printf("|----------------------------\n");
}

void adminhelp_handler()
{
	printf("/----------------------------\n");
	printf("|-----.help:      获取帮助\n");
	printf("|-----.signup:    注册账号\n");
	printf("|-----.signin:    登录其他账号\n");
	printf("|-----.signout:   注销账号\n");
	printf("|-----.rmsign:    删除账号\n");
	printf("|-----.chpswd:    修改账户密码\n");
	printf("|-----.booklist:  获取图书列表\n");
	printf("|-----.bookdown:  下载图书\n");
	printf("|-----.bookup:    上传图书\n");
	printf("|-----.bookdel:   删除图书\n");
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
 * ERRRECV、ERRSEND、ERRBREAK、ERRSERVER、ERRPASSWD、ERRACCNOEXIST
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
		return -1;
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
		return -1;
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

int booklist_handler(int sockfd)
{
	int ret;
	nwbs_proto_t cli_msg, ser_msg;
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	cli_msg.proto_opt = CLIBOOKLIST;
	ret = send(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		return -1;
	}

	printf("/-------------------------------------------------------------------------------|\n");
	printf("|---booknum-----bookname----------------bookauthor--------------bookfilename----|\n");

	while (1) {
		memset(&ser_msg, 0, sizeof(nwbs_proto_t));
		ret = recv(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRRECV);
			return -1;
		} else if (ret == 0) {
			nwbs_seterrno(ERRBREAK);
			return -1;
		}

		if (ser_msg.proto_opt != SERSUCCESS)
			break;
		display_bookinfo(ser_msg.proto_book);
	}

	printf("|-------------------------------------------------------------------------------|\n");

	/* 解析协议 */
	switch (ser_msg.proto_opt) {
		case SERLISTEND:
			nwbs_seterrno(ERRSUCCESS);
			break;
		case SERERROR:
			nwbs_seterrno(ERRSERVER);
			return -1;
	}

	return 0;
}

int bookinfo_handler(int sockfd, nwbs_book_t *book)
{
	int ret;
	nwbs_proto_t ser_msg, cli_msg;
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	cli_msg.proto_opt = CLIBOOKINFO;
	cli_msg.proto_book.booknum = (*book).booknum;
	ret = send(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		return -1;
	}

	ret = recv(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRRECV);
		return -1;
	}

	switch (ser_msg.proto_opt) {
		case SERSUCCESS:
			(*book).booknum = ser_msg.proto_book.booknum;
			strcpy((*book).bookname, ser_msg.proto_book.bookname);
			strcpy((*book).bookauthor, ser_msg.proto_book.bookauthor);
			strcpy((*book).bookfilename, ser_msg.proto_book.bookfilename);
			nwbs_seterrno(ERRSUCCESS);
			break;
		case SERERROR:
			nwbs_seterrno(ERRSERVER);
			return -1;
		case SERNOEXIST:
			nwbs_seterrno(ERRBKNOEXIST);
			return -1;
	}
	return 0;
}

int bookdown_handler(int sockfd, int booknum, const char *bookpath)
{
	int ret;
	FILE *bookfp;
	nwbs_proto_t cli_msg, ser_msg;
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	bookfp = fopen(bookpath, "w");
	if (bookfp == NULL) {
		nwbs_seterrno(ERRFILEOPT);
		return -1;
	}
	cli_msg.proto_opt = CLIBOOKDOWN;
	cli_msg.proto_bookdown.booknum = booknum;
	ret = send(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		goto err1;
	}

	while (1) {
		/* 循环接收图书信息 */
		ret = recv(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRRECV);
			goto err1;
		} else if (ret == 0) {
			nwbs_seterrno(ERRBREAK);
			goto err1;
		}
		/* 如果接收到的协议包不是SERSUCCESS，则跳出循环 */
		if (ser_msg.proto_opt != SERSUCCESS) {
			break;
		}
		/* 如果接收到的是有效数据，则将数据写入到文件中 */
		ret = fwrite(ser_msg.proto_bookdown.bookbuf, sizeof(char), ser_msg.proto_bookdown.bookdatasize, bookfp);
		if (ret < 0) {
			nwbs_seterrno(ERRFILEOPT);
			goto err1;
		}
	}

	switch (ser_msg.proto_opt) {
		case SERDOWNEND:
			nwbs_seterrno(ERRSUCCESS);
			break;
		case SERERROR:
			nwbs_seterrno(ERRSERVER);
			goto err1;
		case SERNOEXIST:
			nwbs_seterrno(ERRBKNOEXIST);
			goto err1;
	}

	fclose(bookfp);
	return 0;

err1:
	unlink(bookpath);
	fclose(bookfp);
	return -1;
}

int bookup_handler(int sockfd)
{
	int ret;
	nwbs_proto_t cli_msg, ser_msg;
	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	/* 发送请求 */
	cli_msg.proto_opt = CLIBOOKUP;
	ret = send(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		goto err1;
	}

	ret = recv(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRRECV);
		goto err1;
	}

	return 0;
err1:
	return -1;
}