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

#define BUFLEN 39

#ifndef __IEEE80211_H_
#define __IEEE80211_H_

struct frame_control
{
	unsigned char to_ds		:1;		
	unsigned char from_ds   :1; 
	unsigned char more_frag :1; 
	unsigned char retry		:1; 
	unsigned char powermant :1; 
	unsigned char moredata  :1; 
	unsigned char protframe :1; 
	unsigned char order		:1; 
	unsigned char version   :2; 
	unsigned char type		:2; 
	unsigned char subtype   :4; 
} __attribute__ ((packed));

struct ieee80211_hdr_3addr 
{
	unsigned short duration_id;
	struct frame_control frame_control;
	unsigned char addr1[6];
	unsigned char addr2[6];
	unsigned char addr3[6];
	unsigned short seq_ctrl;
} __attribute__ ((packed));

#endif

int main(int argc,char** argv)
{
	int sd;
	int ret;
	char buf[BUFLEN]={0};
	
	struct ieee80211_hdr_3addr *wifi;

	struct  sockaddr_ll  toaddr;
	struct ifreq ifr;
	
	unsigned char src_mac[ETH_ALEN]={0x43,0x34,0x4C,0x3B,0x26,0xD4};
	unsigned char dst_mac[ETH_ALEN]={0xff,0xff,0xff,0xff,0xff,0xff}; //全网广播
	unsigned char ap_mac[ETH_ALEN] ={0x6c,0x59,0x40,0xf0,0x63,0x04};

	sd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sd < 0)
	{
		perror("socket()");
		return -1;
	}

	memset(buf, 0, BUFLEN);

	wifi = (struct ieee80211_hdr_3addr *)buf;
	wifi->frame_control.version = 0;
	wifi->frame_control.type = 00;
	wifi->frame_control.subtype = htons(8);
	wifi->frame_control.to_ds = 0;
	wifi->frame_control.from_ds = 0;

	wifi->duration_id = 0;

	memcpy(wifi->addr1, dst_mac, 6);
	memcpy(wifi->addr2, src_mac, 6);
	memcpy(wifi->addr3, dst_mac, 6);

	char *str = "hello wifi";
	memcpy((buf + sizeof(struct ieee80211_hdr_3addr)), str, strlen(str));

	//指定接口
	strcpy(ifr.ifr_name, "wlan0");
	ioctl(sd, SIOCGIFINDEX, &ifr);
	toaddr.sll_ifindex = ifr.ifr_ifindex;	
	toaddr.sll_family = PF_PACKET;

	while(1)
	{
		sendto(sd, buf, BUFLEN, 0, (struct sockaddr*)&toaddr, sizeof(toaddr));

		//sleep(1);
	}

	close(sd);

	return 0;
}



