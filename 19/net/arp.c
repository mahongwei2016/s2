#include "arp.h"
#define EINT0MASK (*((volatile unsigned long *)0x7F008920))
ARP_HDR *arpbuf=txbuffer;
void arp_request(int i)
{
	//1.构建arp请求包
	memcpy(arpbuf->ethhdr.d_mac,host_mac_addr,6); 
	memcpy(arpbuf->ethhdr.s_mac,mac_addr,6);
	arpbuf->ethhdr.type = HON(0x0806);
	arpbuf->hwtype = HON(1);
    arpbuf->protocol = HON(0x0800);
    arpbuf->hwlen = 6;
    arpbuf->protolen = 4;
    arpbuf->opcode = HON(1);
    memcpy(arpbuf->smac,mac_addr,6);
    memcpy(arpbuf->sipaddr,ip_addr,4);
    memcpy(arpbuf->dipaddr,host_ip_addr,4);
    memcpy(arpbuf->dmac,&i,1); 
    txpacket_len = 14+28;
    /*2.调用dm9000发送函数，发送应答包*/	
    //dm9000_tx(txbuffer,txpacket_len);
    printf("arp:send %dbyte\n\r",dm9000_tx(txbuffer,txpacket_len));
    //EINT0MASK&=(~(1<<7));
}


/*2.解析arp应答包，提取mac*/
u8 arp_process(u8* buf,u32 len)
{
    u32 i;
    ARP_HDR* arp_p=(ARP_HDR*)buf;
    if (len<28)
        return 0;
    switch(HON(arp_p->opcode))
    {
		/*收到arp响应包*/
		case 2:
		    memcpy(host_ip_addr,arp_p->sipaddr,4);
		    printf("time:%s\r\n",__TIME__);
		    printf("host ip is : ");
		    for(i=0;i<4;i++)
		        printf("%03d ",host_ip_addr[i]);
		    printf("\n\r");
		    memcpy(host_mac_addr,arp_p->smac,6);
		    printf("host mac is : ");
		    for(i=0;i<6;i++)
		        printf("%02x ",host_mac_addr[i]);
	    	printf("\n\r");
	    	break;
	    //发送arp响应包 
	    case 1:
			memcpy(arpbuf->ethhdr.d_mac,arp_p->smac,6); 
			memcpy(arpbuf->ethhdr.s_mac,mac_addr,6);
			arpbuf->ethhdr.type = HON(0x0806);
			arpbuf->hwtype = HON(1);
			arpbuf->protocol = HON(0x0800);
		    arpbuf->hwlen = 6;
		    arpbuf->protolen = 4;
		    arpbuf->opcode = HON(2);
		    memcpy(arpbuf->smac,mac_addr,6);
		    memcpy(arpbuf->sipaddr,ip_addr,4);
		    memcpy(arpbuf->dipaddr,arp_p->sipaddr,4);
		    memcpy(arpbuf->dmac,arp_p->smac,6); 
		    txpacket_len = 14+28;
		    printf("arp:send %dbyte\n\r",dm9000_tx(txbuffer,txpacket_len));
	    	break;
	}

}

void udp_process(u8* buf, u32 len)
{
     UDP_HDR *udphdr = (UDP_HDR *)buf;
     
     tftp_process(buf,len,HON(udphdr->sport));     	
}

void ip_process(u8 *buf, u32 len)
{
     IP_HDR *p = (IP_HDR *)buf;	
     
     switch(p->proto)
     {
         case PROTO_UDP:
			 printf("udp \n\r");
             udp_process(buf,len);
             break;
             
         default:
             break;	
     }
}

void net_handle(u8* buf,u32 len) 
{
	ETH_HDR *p=(ETH_HDR *)buf;
	switch(HON(p->type))
	{
		case PROTO_ARP:
				printf("arp \n\r");
				arp_process(buf,len);
				break;
		case PROTO_IP:
				printf("ip \n\r");
				ip_process(buf,len);
				break;
		default:
			break;
	}
}
