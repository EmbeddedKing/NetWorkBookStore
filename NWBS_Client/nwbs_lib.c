#include "nwbs_lib.h"

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
		return -1;
	}
	return 0;
}

/**
 * 该函数用来关闭回显
 */
void echo_off()
{
	struct termios oldtermios, newtermios;
	tcgetattr(STDIN_FILENO, &oldtermios);
	newtermios = oldtermios;
	newtermios.c_lflag &= ~ECHO;	//取消禁止从终端输入的字符送入输出队列，只送入输出队列
	tcsetattr(STDIN_FILENO,TCSANOW,&newtermios);
}

/**
 * 该函数用来打开回显
 */
void echo_on()
{
	struct termios oldtermios, newtermios;
	tcgetattr(STDIN_FILENO, &oldtermios);
	newtermios = oldtermios;
	newtermios.c_lflag |= ECHO | ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
}

/**
 * 该函数用来显示图书信息
 * @param book 图书信息结构体
 */
void display_bookinfo(nwbs_book_t book)
{
	printf("|      %d\t%s\t\t%s\t\t%s\t|\n"
			, book.booknum
			, book.bookname
			, book.bookauthor
			, book.bookfilename);
}

/**
 * 该函数用来支持退格符
 */
void set_backspace()
{
	struct termios newtermios;
	tcgetattr(STDIN_FILENO, &newtermios);
    newtermios.c_cc[VERASE] = '\b';
    tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
}
