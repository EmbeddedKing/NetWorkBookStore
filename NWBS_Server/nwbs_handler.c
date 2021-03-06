#include "nwbs_handler.h"

/*
 * 该函数用来解析协议包解析协议包
 */
int msg_handler(int sockfd, nwbs_proto_t cli_msg)
{
	switch(cli_msg.proto_opt) {
		/* 成功 */
		case CLISUCCESS:
			success_handler(sockfd, cli_msg);
			printf("%s", nwbs_error());
			break;
		/* 注册账号请求 */
		case CLISIGNUP:
			signup_handler(sockfd, cli_msg);
			if (nwbs_geterrno())
				printf("%s", nwbs_error());
			break;
		/* 登录请求 */
		case CLISIGNIN:
			signin_handler(sockfd, cli_msg);
			if (nwbs_geterrno())
				printf("%s", nwbs_error());
			break;
		/* 修改密码请求 */
		case CLICHPSWD:
			chpswd_handler(sockfd, cli_msg);
			if (nwbs_geterrno())
				printf("%s", nwbs_error());
			break;
		/* 获取图书列表 */
		case CLIBOOKLIST:
			booklist_handler(sockfd, cli_msg);
			break;
		/* 下载图书 */
		case CLIBOOKDOWN:
			bookdown_handler(sockfd, cli_msg);
			break;
		/* 获取图书信息 */
		case CLIBOOKINFO:
			bookinfo_handler(sockfd, cli_msg);
			break;
		/* 上传图书 */
		case CLIBOOKUP:
			bookup_handler(sockfd, cli_msg);
			break;
		case CLIBOOKDEL:
			bookdel_handler(sockfd, cli_msg);
			break;
		/* 退出请求 */
		case CLIEXIT:
			exit_handler();
			if (nwbs_geterrno())
				printf("%s", nwbs_error());
			exit(0);
		default:
			printf("不知道的协议类型\n");
			break;
	}
}

/*
 * 该函数用来做操作成功处理，预留
 */
int success_handler(int sockfd, nwbs_proto_t cli_msg)
{
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));
}

/*
 * 该函数用来处理客户端注册操作
 * 成功返回0，失败返回-1并设置nwbs_errno
 */
int signup_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	MYSQL sql_db;
	MYSQL_RES *sql_res;
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));
	/* 初始化并连接mysql数据库 */
	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err3;
	}

	char sql_com[200] = {0};
	sprintf(sql_com, "select * from user_info where useraccount = '%s';"
			, cli_msg.proto_user.useraccount);
	/* 搜索数据库中有没有该用户 */
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		goto err2;
	}

	/* 查看搜索到的信息 */
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		/* 如果找到重复的用户返回SEREXIST信息 */
		ser_msg.proto_opt = SEREXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	} else {
		/* 如果没有就将用户信息插入到user_info表中 */
		bzero(sql_com, sizeof(sql_com));
		sprintf(sql_com, "insert into user_info values('%s', '%s', '%s');"
				, cli_msg.proto_user.username
				, cli_msg.proto_user.useraccount
				, cli_msg.proto_user.userpasswd);
		ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
		if (ret < 0) {
			nwbs_seterrno(ERRSQLQUERY);
			goto err1;
		}

		ser_msg.proto_opt = SERSUCCESS;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	}
	/* 关闭结果 */
	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;

err1:
	mysql_free_result(sql_res);
err2:
	mysql_close(&sql_db);
err3:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

/*
 * 该函数用来处理客户端登陆操作
 * 成功返回0，失败返回-1并设置nwbs_errno
 */
int signin_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));
	MYSQL sql_db;
	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err3;
	}

	/* 在数据库中搜索该用户 */
	char sql_com[200] = {0};
	sprintf(sql_com, "select * from user_info where useraccount = '%s';"
			, cli_msg.proto_user.useraccount);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		goto err2;
	}

	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		/* 匹配密码 */
		sql_row = mysql_fetch_row(sql_res);

		if (strcmp(cli_msg.proto_user.userpasswd, sql_row[DBUSERPASSWD])) {
			/* 匹配失败，发送密码不匹配 */
			ser_msg.proto_opt = SERERRPSWD;
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				goto err1;
			}
		} else {
			/* 匹配成功，把用户名发送过去 */
			ser_msg.proto_opt = SERSUCCESS;
			strcpy(ser_msg.proto_user.username, sql_row[DBUSERNAME]);
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				goto err1;
			}
		}
	} else {
		/* 发送用户没有找到 */
		ser_msg.proto_opt = SERNOEXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	}
	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;

err1:
	mysql_free_result(sql_res);
err2:
	mysql_close(&sql_db);
err3:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

/*
 * 该函数用来处理客户端修改密码操作
 * 成功返回0，失败返回-1并设置nwbs_errno
 */
int chpswd_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	MYSQL sql_db;
	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err3;
	}

	char sql_com[200] = {0};
	sprintf(sql_com, "select * from user_info where useraccount = '%s';"
			, cli_msg.proto_chpswd.useraccount);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err2;
	}

	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		/* 找到用户进行旧密码匹配 */
		sql_row = mysql_fetch_row(sql_res);
		if (strcmp(cli_msg.proto_chpswd.oldpasswd, sql_row[DBUSERPASSWD])) {
			/* 没有匹配成功 */
			ser_msg.proto_opt = SERERRPSWD;
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				goto err1;
			}
		} else {
			/* 匹配成功，发送匹配成功消息，并将密码update */
			bzero(sql_com, sizeof(sql_com));
			sprintf(sql_com, "update user_info set userpasswd = '%s' where useraccount = '%s';"
					, cli_msg.proto_chpswd.newpasswd
					, cli_msg.proto_chpswd.useraccount);
			ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
			if (ret < 0) {
				nwbs_seterrno(ERRSQLINIT);
				goto err1;
			}
			ser_msg.proto_opt = SERSUCCESS;
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				goto err1;
			}
		}
	} else {
		/* 通知用户没有找到该用户 */
		ser_msg.proto_opt = SERNOEXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	}

	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;

err1:
	mysql_free_result(sql_res);
err2:
	mysql_close(&sql_db);
err3:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

/*
 * 该函数用来处理客户端图书列表操作
 * 成功返回0，失败返回-1并设置nwbs_errno
 */
int booklist_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	MYSQL sql_db;
	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err3;
	}

	char sql_com[200];
	sprintf(sql_com, "select * from book_info;");
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		goto err2;
	}

	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	while (sql_row = mysql_fetch_row(sql_res)) {
		memset(&ser_msg, 0, sizeof(nwbs_proto_t));
		ser_msg.proto_opt = SERSUCCESS;
		ser_msg.proto_book.booknum = atoi(sql_row[DBBOOKNUM]);
		strcpy(ser_msg.proto_book.bookname, sql_row[DBBOOKNAME]);
		strcpy(ser_msg.proto_book.bookauthor, sql_row[DBBOOKAUTHOR]);
		strcpy(ser_msg.proto_book.bookfilename, sql_row[DBBOOKFILENAME]);
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	}

	ser_msg.proto_opt = SERLISTEND;
	ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	if (ret < 0) {
		nwbs_seterrno(ERRSEND);
		goto err1;
	}

	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;

err1:
	mysql_free_result(sql_res);
err2:
	mysql_close(&sql_db);
err3:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

int bookdown_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	nwbs_proto_t ser_msg;
	MYSQL sql_db;
	FILE *bookfp;

	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err4;
	}

	char sql_com[200];
	sprintf(sql_com, "select * from book_info where booknum = %d;"
			, cli_msg.proto_bookdown.booknum);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		goto err3;
	}

	char bookpath[200] = {0};
	int count;
	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		sql_row = mysql_fetch_row(sql_res);

		strcpy(bookpath, sql_row[DBBOOKPATH]);
		bookpath[strlen(bookpath)] = '/';
		strcpy(bookpath + strlen(bookpath), sql_row[DBBOOKFILENAME]);

		bookfp = fopen(bookpath, "r");
		while (1) {
			memset(&ser_msg, 0, sizeof(nwbs_proto_t));
			count = fread(ser_msg.proto_bookdown.bookbuf, sizeof(char), BUF_SIZE, bookfp);
			if (count == 0) {
				break;
			} else if (count < 0) {
				nwbs_seterrno(ERRFILEOPT);
				goto err1;
			}
			ser_msg.proto_opt = SERSUCCESS;
			ser_msg.proto_bookdown.bookdatasize = count;
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				goto err1;
			}
		}
		ser_msg.proto_opt = SERDOWNEND;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	} else {
		ser_msg.proto_opt = SERNOEXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err2;
		}
	}

	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;

err1:
	fclose(bookfp);
err2:
	mysql_free_result(sql_res);
err3:
	mysql_close(&sql_db);
err4:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

int bookinfo_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	nwbs_proto_t ser_msg;
	MYSQL sql_db;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err3;
	}

	char sql_com[200];
	sprintf(sql_com, "select * from book_info where booknum = %d;"
			, cli_msg.proto_book.booknum);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		goto err2;
	}

	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		sql_row = mysql_fetch_row(sql_res);
		ser_msg.proto_opt = SERSUCCESS;
		ser_msg.proto_book.booknum = atoi(sql_row[DBBOOKNUM]);
		strcpy(ser_msg.proto_book.bookname, sql_row[DBBOOKNAME]);
		strcpy(ser_msg.proto_book.bookauthor, sql_row[DBBOOKAUTHOR]);
		strcpy(ser_msg.proto_book.bookfilename, sql_row[DBBOOKFILENAME]);
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	} else {
		ser_msg.proto_opt = SERNOEXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	}

	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;

err1:
	mysql_free_result(sql_res);
err2:
	mysql_close(&sql_db);
err3:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

int bookup_handler(int sockfd, nwbs_proto_t cli_msg)
{
	FILE *bookfp;
	int ret;
	nwbs_proto_t ser_msg;
	MYSQL sql_db;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	nwbs_book_t book;
	book = cli_msg.proto_book;

#if __DEBUG_MODE
	printf("\n*********************************************\n");
	printf("协议包类型：%d\n", cli_msg.proto_opt);
	printf("书籍名称：%s\n", cli_msg.proto_book.bookname);
	printf("书籍作者：%s\n", cli_msg.proto_book.bookauthor);
	printf("书籍文件名称：%s\n", cli_msg.proto_book.bookfilename);
	printf("*********************************************\n");
#endif

    /* 打开该文件 */
	char bookpath[200] = {0};
	sprintf(bookpath, "./books/%s", cli_msg.proto_book.bookfilename);
	bookfp = fopen(bookpath, "w");
	if (bookfp == NULL) {
		nwbs_seterrno(ERRFILEOPT);
		goto err4;
	}

#if __DEBUG_MODE
	printf("\n*********************************************\n");
	printf("书籍保存路径：%s\n", bookpath);
	printf("*********************************************\n");
#endif

	/* 打开数据库 */
	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err3;
	}
	char sql_com[200];
	sprintf(sql_com, "select * from book_info where bookname = '%s' and bookauthor = '%s';"
			, cli_msg.proto_book.bookname
			, cli_msg.proto_book.bookauthor);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		goto err2;
	}

	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);

    if (num == 0) {
    	/* 如果没有找到重复的则发送服务器已经全部准备好 */
    	ser_msg.proto_opt = SERSUCCESS;
    	ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
    	if (ret < 0) {
    		nwbs_seterrno(ERRSEND);
    		goto err1;
    	}
    } else {
    	/* 如果找到了则发送数据已经存在并返回 */
    	ser_msg.proto_opt = SEREXIST;
    	ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
    	if (ret < 0) {
    		nwbs_seterrno(ERRSEND);
    		goto err1;
    	}
    	return 0;
    }

    while (1) {
    	/* 循环接收书籍数据 */
    	memset(&cli_msg, 0, sizeof(nwbs_proto_t));
    	ret = recv(sockfd, &cli_msg, sizeof(nwbs_proto_t), 0);
    	if (ret < 0) {
    		nwbs_seterrno(ERRRECV);
    		goto err1;
    	}

    	if (cli_msg.proto_opt != CLISUCCESS) {
    		break;
    	} else {
    		ser_msg.proto_opt = SERSUCCESS;
    		send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
    	}
    	/* 将数据写入到文件中 */
    	fwrite(cli_msg.proto_bookdown.bookbuf, sizeof(char), cli_msg.proto_bookdown.bookdatasize , bookfp);
    }

    switch (cli_msg.proto_opt) {
    	case CLIUPEND:
    		bzero(sql_com, sizeof(sql_com));
    		sprintf(sql_com, "insert into book_info(bookname, bookauthor, bookfilename, bookpath) \
    				values('%s', '%s', '%s', './books');"
    				, book.bookname
    				, book.bookauthor
    				, book.bookfilename);
    		ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
    		if (ret < 0) {
    			unlink(bookpath);
    			nwbs_seterrno(ERRSQLQUERY);
    			return -1;
    		}
    		nwbs_seterrno(ERRSUCCESS);
    		break;
    	case CLIERROR:
    		nwbs_seterrno(ERRCLIENT);
    		goto err1;
    }

    mysql_free_result(sql_res);
	mysql_close(&sql_db);
	fclose(bookfp);
    return 0;

err1:
	mysql_free_result(sql_res);
err2:
	mysql_close(&sql_db);
err3:
	fclose(bookfp);
	unlink(bookpath);
err4:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

int bookdel_handler(int sockfd, nwbs_proto_t cli_msg)
{
#if __DEBUG_MODE
	printf("\n*********************************************\n");
	printf("要删除的图书编号为：%d\n", cli_msg.proto_book.booknum);
	printf("*********************************************\n");
#endif
	int ret;
	nwbs_proto_t ser_msg;
	MYSQL sql_db;

	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		goto err3;
	}

	char sql_com[200] = {0};
	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sprintf(sql_com, "select * from book_info where booknum = %d;"
			, cli_msg.proto_book.booknum);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		goto err2;
	}

	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		/* 如果找到图书，则将图书做删除操作 */
		bzero(sql_com, sizeof(sql_com));
		sprintf(sql_com, "delete from book_info where booknum = %d;"
				, cli_msg.proto_book.booknum);
		sql_row = mysql_fetch_row(sql_res);
		/* 删除对应的本地文件 */
		char bookpath[200] = {0};
		sprintf(bookpath, "%s/%s", sql_row[DBBOOKPATH], sql_row[DBBOOKFILENAME]);
		unlink(bookpath);
		ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
		if (ret < 0) {
			nwbs_seterrno(ERRSQLQUERY);
			goto err1;
		}
		memset(&ser_msg, 0, sizeof(nwbs_proto_t));
		ser_msg.proto_opt = SERSUCCESS;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
		//如果是删除的最后一本图书，则将自增改为1
		if (num == 1) {
			bzero(sql_com, sizeof(sql_com));
			sprintf(sql_com, "alter table book_info auto_increment = 1;");
			ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
			if (ret < 0) {
				nwbs_seterrno(ERRSQLQUERY);
				goto err1;
			}
		}
	} else {
		memset(&ser_msg, 0, sizeof(nwbs_proto_t));
		ser_msg.proto_opt = SERNOEXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			goto err1;
		}
	}

	mysql_close(&sql_db);
	mysql_free_result(sql_res);
	return 0;

err1:
	mysql_free_result(sql_res);
err2:
	mysql_close(&sql_db);
err3:
	ser_msg.proto_opt = SERERROR;
	send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
	return -1;
}

/*
 * 该函数预留用来做退出处理
 */
int exit_handler()
{

}
