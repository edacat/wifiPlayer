#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <net/ethernet.h>

#define BUFLEN 60

int main(int argc,char** argv)
{
	int sd;
	int ret;
	char buf[BUFLEN]={0};
	
	struct ethhdr *eth;

	struct  sockaddr_ll  toaddr;
	struct ifreq ifr;
	
	unsigned char src_mac[ETH_ALEN]={0x00,0x0c,0x29,0x01,0x31,0xcd};
	unsigned char dst_mac[ETH_ALEN]={0xff,0xff,0xff,0xff,0xff,0xff}; //全网广播
	
	sd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sd < 0)
	{
		perror("socket error\n");
		return -1;
	}

	//开始填充，构造以太头部
	eth = (struct ethhdr*)buf;
	memcpy(eth->h_dest, dst_mac, ETH_ALEN); 
	memcpy(eth->h_source, src_mac, ETH_ALEN);
	eth->h_proto = htons(ETHERTYPE_ARP);

	char *str = "hello world";
	memcpy((buf + sizeof(struct ether_header)), str, strlen(str));

	//填充目的端的IP地址，MAC地址不用管
	//inet_pton(AF_INET, "255.255.255.255", &toaddr);
	strcpy(ifr.ifr_name, "eth0");
	ioctl(sd, SIOCGIFINDEX, &ifr);
	toaddr.sll_ifindex = ifr.ifr_ifindex;	
	toaddr.sll_family = PF_PACKET;

	sendto(sd, buf, BUFLEN, 0, (struct sockaddr*)&toaddr, sizeof(toaddr));

	close(sd);

	return 0;
}



