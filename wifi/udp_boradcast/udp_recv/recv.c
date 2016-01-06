#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
	//绑定地址
	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family = AF_INET;
	addrto.sin_addr.s_addr = inet_addr("255.255.255.255");
	addrto.sin_port = htons(6000);

	struct sockaddr_in from;

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
		perror("set socket error...");
		return -1;
	}

	if(bind(sock,(struct sockaddr *)&(addrto), sizeof(struct sockaddr_in)) == -1) 
	{   
		perror("bind()");
		return -1;
	}

	int len = sizeof(struct sockaddr_in);
	char smsg[100] = {0};

	while(1)
	{
		//从广播地址接受消息
		int ret=recvfrom(sock, smsg, 100, 0, (struct sockaddr *)&from, (socklen_t*)&len);
		if(ret<=0)
		{
			perror("recvfrom()");
		}
		else
		{		
			printf("%s\n", smsg);	
		}
	}

	return 0;
}



