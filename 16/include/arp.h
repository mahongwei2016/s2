#define HON(n) ((((u16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef struct eth_hdr
{
    u8 d_mac[6];
    u8 s_mac[6];
    u16 type;
}ETH_HDR;
typedef struct arp_hdr
{
    ETH_HDR ethhdr;
    u16 hwtype;
    u16 protocol;
    u8 hwlen;
    u8 protolen;
    u16 opcode;
    u8 smac[6];
    u8 sipaddr[4];
    u8 dmac[6];
    u8 dipaddr[4];
}ARP_HDR;
extern ARP_HDR arpbuf;
extern u8 host_mac_addr[6];
extern u8 mac_addr[6];
extern u8 ip_addr[4];
extern u8 host_ip_addr[4];
extern u16 txpacket_len;
extern u16 rxpacket_len;

extern unsigned char *txbuffer;
extern unsigned char rxbuffer[1522];
