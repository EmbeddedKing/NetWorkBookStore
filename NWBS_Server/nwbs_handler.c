#include "nwbs_handler.h"

/* 解析协议包 */
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

int success_handler(int sockfd, nwbs_proto_t cli_msg)
{
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));
}

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
		ser_msg.proto_opt = SERERROR;
		send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		return -1;
	}

	char sql_com[200] = {0};
	sprintf(sql_com, "select * from user_info where useraccount = '%s';"
			, cli_msg.proto_user.useraccount);
	/* 搜索数据库中有没有该用户 */
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		ser_msg.proto_opt = SERERROR;
		send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		return -1;
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
			return -1;
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
			ser_msg.proto_opt = SERERROR;
			send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			return -1;
		}

		ser_msg.proto_opt = SERSUCCESS;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			return -1;
		}
	}
	/* 关闭结果 */
	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;
}

int signin_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));
	MYSQL sql_db;
	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		ser_msg.proto_opt = SERERROR;
		send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		return -1;
	}

	/* 在数据库中搜索该用户 */
	char sql_com[200] = {0};
	sprintf(sql_com, "select * from user_info where useraccount = '%s';"
			, cli_msg.proto_user.useraccount);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLQUERY);
		ser_msg.proto_opt = SERERROR;
		send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		return -1;
	}

	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		/* 匹配密码 */
		sql_row = mysql_fetch_row(sql_res);
		if (strcmp(cli_msg.proto_user.userpasswd, sql_row[2])) {
			/* 匹配失败，发送密码不匹配 */
			ser_msg.proto_opt = SERERRPSWD;
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				return -1;
			}
		} else {
			/* 匹配成功，把用户名发送过去 */
			ser_msg.proto_opt = SERSUCCESS;
			strcpy(ser_msg.proto_user.username, sql_row[0]);
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				return -1;
			}
		}
	} else {
		/* 发送用户没有找到 */
		ser_msg.proto_opt = SERNOEXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			return -1;
		}
	}
	mysql_free_result(sql_res);
	mysql_close(&sql_db);
	return 0;
}

int chpswd_handler(int sockfd, nwbs_proto_t cli_msg)
{
	int ret;
	nwbs_proto_t ser_msg;
	memset(&ser_msg, 0, sizeof(nwbs_proto_t));

	MYSQL sql_db;
	ret = sql_init(&sql_db);
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		ser_msg.proto_opt = SERERROR;
		send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		return -1;
	}

	char sql_com[200] = {0};
	sprintf(sql_com, "select * from user_info where useraccount = '%s';"
			, cli_msg.proto_chpswd.useraccount);
	ret = mysql_real_query(&sql_db, sql_com, strlen(sql_com));
	if (ret < 0) {
		nwbs_seterrno(ERRSQLINIT);
		ser_msg.proto_opt = SERERROR;
		send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		return -1;
	}

	MYSQL_RES *sql_res;
	MYSQL_ROW sql_row;
	sql_res = mysql_store_result(&sql_db);
	int num = mysql_num_rows(sql_res);
	if (num) {
		/* 找到用户进行旧密码匹配 */
		sql_row = mysql_fetch_row(sql_res);
		if (strcmp(cli_msg.proto_chpswd.oldpasswd, sql_row[2])) {
			/* 没有匹配成功 */
			ser_msg.proto_opt = SERERRPSWD;
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				return -1;
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
				ser_msg.proto_opt = SERERROR;
				send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
				return -1;
			}
			ser_msg.proto_opt = SERSUCCESS;
			ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
			if (ret < 0) {
				nwbs_seterrno(ERRSEND);
				return -1;
			}
		}
	} else {
		/* 通知用户没有找到该用户 */
		ser_msg.proto_opt = SERNOEXIST;
		ret = send(sockfd, &ser_msg, sizeof(nwbs_proto_t), 0);
		if (ret < 0) {
			nwbs_seterrno(ERRSEND);
			return -1;
		}
	}
	mysql_free_result(sql_res);
	mysql_close(&sql_db);
}

int exit_handler()
{
}

/* 初始化并连接连接数据库 */
int sql_init(MYSQL *sql_db)
{
	mysql_init(sql_db);
	if (mysql_errno(sql_db)) {
		printf("mysql_init:%s\n", mysql_error(sql_db));
		return -1;
	}

	mysql_real_connect(sql_db, "localhost", "root", "123", "NWBS", 0, NULL, 0);
	if (mysql_errno(sql_db)) {
		printf("mysql_real_connect:%s\n", mysql_error(sql_db));
		return -1;
	}
}