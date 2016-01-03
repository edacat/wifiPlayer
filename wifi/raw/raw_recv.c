
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

#define BUFLEN 60

int main(int argc,char** argv)
{
	int i,skfd,n;

	char buf[ETH_FRAME_LEN]={0};

	struct ethhdr *eth;

	struct sockaddr_ll fromaddr;
	struct ifreq ifr;

	unsigned char src_mac[ETH_ALEN]={0};

	//只接收发给本机的ARP报文
	skfd=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	if(skfd < 0)
	{
		perror("socket()");
		return -1;
	}

	bzero(&fromaddr,sizeof(fromaddr));
	bzero(&ifr,sizeof(ifr));
	strcpy(ifr.ifr_name, "eth0");
	ioctl(skfd, SIOCGIFINDEX, &ifr);
	fromaddr.sll_ifindex = ifr.ifr_ifindex;
	fromaddr.sll_family = PF_PACKET;
	bind(skfd, (struct sockaddr*)&fromaddr,sizeof(struct sockaddr));

	while(1)
	{
		memset(buf, 0, ETH_FRAME_LEN);

		recvfrom(skfd, buf,ETH_FRAME_LEN,0,NULL,NULL);

		eth=(struct ethhdr*)buf;
		if(eth->h_proto == htons(ETHERTYPE_ARP))
		{

			printf("src mac = %02x:%02x:%02x:%02x:%02x:%02x\n",
					eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);

			printf("dst mac = %02x:%02x:%02x:%02x:%02x:%02x\n",
					eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

			printf("proto = %d\n", eth->h_proto);
			printf("data = %s\n", (char *)(buf + sizeof(struct ethhdr)));
		}
	}

	close(skfd);

	return 0;

}



