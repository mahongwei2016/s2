#include "arp.h"
#define EINT0MASK (*((volatile unsigned long *)0x7F008920))
ARP_HDR arpbuf;
void arp_request(int i)
{
	//1.构建arp请求包
	memcpy(arpbuf.ethhdr.d_mac,host_mac_addr,6); 
	memcpy(arpbuf.ethhdr.s_mac,mac_addr,6);
	arpbuf.ethhdr.type = HON(0x0806);
	arpbuf.hwtype = HON(1);
    arpbuf.protocol = HON(0x0800);
    arpbuf.hwlen = 6;
    arpbuf.protolen = 4;
    arpbuf.opcode = HON(1);
    memcpy(arpbuf.smac,mac_addr,6);
    memcpy(arpbuf.sipaddr,ip_addr,4);
    memcpy(arpbuf.dipaddr,host_ip_addr,4);
    memcpy(arpbuf.dmac,&i,1); 
    txpacket_len = 14+28;
    /*2.调用dm9000发送函数，发送应答包*/	
    //dm9000_tx(txbuffer,txpacket_len);
    printf("arp:send %dbyte\n\r",dm9000_tx(txbuffer,txpacket_len));
    //EINT0MASK&=(~(1<<7));
}


/*2.解析arp应答包，提取mac*/
u8 arp_process()
{

    u32 i;
    
    if (rxpacket_len<28)
        return 0;
    ARP_HDR *arprxbuf=rxbuffer;
    memcpy(host_ip_addr,arprxbuf->sipaddr,4);
    printf("host ip is : ");
    for(i=0;i<4;i++)
        printf("%03d ",host_ip_addr[i]);
    printf("\n\r");
    
    memcpy(host_mac_addr,arprxbuf->smac,6);
    printf("host mac is : ");
    for(i=0;i<6;i++)
        printf("%02x ",host_mac_addr[i]);
    printf("\n\r");

}
