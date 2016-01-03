#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const int C = 0;

struct sockaddr_in addrto;
int nlen = sizeof(addrto);
static int sd;


int init_socket()
{
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
	{   
		perror("socket error");	
		return -1;
	}   

	const int opt = 1;
	//设置该套接字为广播类型，
	int nb = 0;
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		perror("setsockopt()");
		return -1;
	}

	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = inet_addr("255.255.255.255");
	addrto.sin_port = htons(6000);

	return sock;
}

/*
 * 密文长度  映射释义
 * 1257	== 起始符; 连续的3个起始符, 用于表示数据传输开始
 * 1258	== 结束符; 连续的3个结束符, 用于表示数据传输结束
 * 1259	== 间隔符; 连续的2个间隔符, 用于表示数据符之间的间隔
 * 
 * 1000	==	数据符; 表示 ASCII 0x00
 * 1001	==	数据符; 表示 ASCII 0x01
 * ...
 * 1255	==  数据符; 表示 ASCII 0xff
 * */

int send_len(int len)
{
	char msg[len];

	memset(msg, 0, len);
	int ret = sendto(sd, msg, len, 0, (struct sockaddr *)&addrto, nlen);
	printf("send len : %d\n", len);
	if(ret<0)
	{
		perror("sendto()");
		return -1;
	}

	return 1;
}

int send_char(char ch)
{
	int len = ch + 1000 + C;

	return send_len(len);
}

/* 99	== 起始符; 连续的3个起始符, 用于表示数据传输开始*/
int send_start()
{
	return send_len(1257 + C);
}

/* 88	== 结束符; 连续的3个结束符, 用于表示数据传输结束*/
int send_end()
{
	return send_len(1258 + C);
}

/* 77	== 间隔符; 连续的2个间隔符, 用于表示数据符之间的间隔*/
int send_div()
{
	return send_len(1259 + C);
}

int send_string(char *str)
{
	send_start();
	while(*str != '\0')
	{
		printf("%c", *str);
		send_div();
		send_char(*str);
		str++;
	}
	send_end();

	return 0;
}

int main()
{
	int i = 10;

	sd = init_socket();
	if(sd < 0)
	{
		return -1;
	}

	while(1)
	{
		send_string("hello");
	}

	close(sd);

	return 0;
}

