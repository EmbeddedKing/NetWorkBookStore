#include "nwbs_lib.h"

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

/*
 * 该函数用来初始化数据库句柄并连接数据库
 */
int sql_init(MYSQL *sql_db)
{
	mysql_init(sql_db);
	if (mysql_errno(sql_db)) {
		printf("mysql_init:%s\n", mysql_error(sql_db));
		return -1;
	}

	mysql_real_connect(sql_db, ser_dbinfo.dbhost, ser_dbinfo.dbuser, ser_dbinfo.dbpasswd, ser_dbinfo.dbname, 0, NULL, 0);
	if (mysql_errno(sql_db)) {
		printf("mysql_real_connect:%s\n", mysql_error(sql_db));
		return -1;
	}
}