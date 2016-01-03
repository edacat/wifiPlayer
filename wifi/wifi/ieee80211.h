#ifndef __IEEE80211_H_
#define __IEEE80211_H_

struct frame_control
{
	unsigned char version	:2;
	unsigned char type	:2;
	unsigned char subtype	:4;
	unsigned char to_ds	:1;
	unsigned char from_ds	:1;
	unsigned char more_frag	:1;
	unsigned char retry	:1;
	unsigned char powermant	:1;
	unsigned char moredata	:1;
	unsigned char protframe	:1;
	unsigned char order	:1;
} __attribute__ ((packed));

struct ieee80211_hdr_3addr 
{
	struct frame_control frame_control;
	unsigned short duration_id;
	unsigned char addr1[6];
	unsigned char addr2[6];
	unsigned char addr3[6];
	unsigned short seq_ctrl;
} __attribute__ ((packed));

#endif
