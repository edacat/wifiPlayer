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

#define BUFLEN 42

struct beacon 
{
	unsigned int timestamp[2];
	unsigned short beacon_int;
	unsigned short capab_info;
	/* followed by some of SSID, Supported rates,
	 * FH Params, DS Params, CF Params, IBSS Params, TIM */
	unsigned char variable[0];
} __attribute__ ((packed));

struct ieee80211_hdr_3addr 
{
	unsigned short frame_control;
	unsigned short duration_id;
	unsigned char addr1[6];
	unsigned char addr2[6];
	unsigned char addr3[6];
	unsigned short seq_ctrl;
} __attribute__ ((packed));

int main(int argc,char** argv){
	
	int skfd;
	int ret;

	char buf[BUFLEN]={0};
	
	struct ifreq ifr;

	unsigned char src_mac[ETH_ALEN]={0};
	unsigned char dst_mac[ETH_ALEN]={0xff,0xff,0xff,0xff,0xff,0xff}; //全网广播

	struct sockaddr_in toaddr;

	if(2 != argc)
	{
		printf("Usage: %s netdevName\n",argv[0]);
		exit(1);
	}

	skfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(skfd < 0)
	{
		perror("socket()");
		return -1;
	}

	bzero(&ifr,sizeof(ifr));
	strcpy(ifr.ifr_name, argv[1]);

	//获取接口索引
	if(-1 == ioctl(skfd,SIOCGIFINDEX,&ifr))
	{
		perror("get dev index error:");
		return -1;
	}
	printf("interface index: %d\n",ifr.ifr_ifindex);

	//获取接口的MAC地址
	if(-1 == ioctl(skfd, SIOCGIFHWADDR, &ifr))
	{
		perror("get dev mac addr error:");
		return -1;
	}

	memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	printf("MAC :%02X-%02X-%02X-%02X-%02X-%02X\n",src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);

	//开始填充，构造以太头部
	//eth=(struct ether_header*)buf;
	//memcpy(eth->ether_dhost,dst_mac,ETH_ALEN); 
	//memcpy(eth->ether_shost,src_mac,ETH_ALEN);
	//eth->ether_type = htons(ETHERTYPE_ARP);

	//手动开始填充用ARP报文首部
	//arp=(struct arphdr*)(buf + sizeof(struct ether_header));
	//arp->arp_hrd = htons(ARPHRD_ETHER); //硬件类型为以太
	//arp->arp_pro = htons(ETHERTYPE_IP); //协议类型为IP

	//硬件地址长度和IPV4地址长度分别是6字节和4字节
	//arp->arp_hln = ETH_ALEN;
	//arp->arp_pln = 4;

	//操作码，这里我们发送ARP请求
	//arp->arp_op = htons(ARPOP_REQUEST);

	//填充发送端的MAC和IP地址
	//memcpy(arp->arp_sha,src_mac,ETH_ALEN);
	//memcpy(arp->arp_spa,&srcIP,4);

	//填充目的端的IP地址，MAC地址不用管
	inet_pton(AF_INET, "255.255.255.255", &toaddr.sin_addr);
	toaddr.sin_family = PF_PACKET;

	ret = sendto(skfd, buf, BUFLEN, 0, (struct sockaddr *)&toaddr, sizeof(toaddr));

	close(skfd);

	return 0;
}

