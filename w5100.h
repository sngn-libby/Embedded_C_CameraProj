#ifndef	__ETHERNET_H__
#define __ETHERNET_H__

#include "2440addr.h"
#include <string.h>
/*
typedef unsigned char SOCKET;

typedef unsigned int 		U32;
typedef signed int			S32;
typedef unsigned short		U16;
typedef short int			S16;
typedef unsigned char		U8;
typedef signed char 		S8;
typedef unsigned long long 	ULL;
*/

//	Ethernet
#define ETHER_BASE	0x08000000

#define	SEND_DATA_BUF	0x08004000			// Internal Tx buffer address of W3100A
#define	RECV_DATA_BUF	0x08006000			// Internal Rx buffer address of W3100A


//	Set W5100 Register Offset
#define MR ETHER_BASE
#define IDM_OR ((ETHER_BASE + 0x00))
#define IDM_AR0 ((ETHER_BASE + 0x01))
#define IDM_AR1 ((ETHER_BASE + 0x02))
#define IDM_DR ((ETHER_BASE + 0x03))


/**
 @brief Gateway IP Register address
 */
#define GAR0				(ETHER_BASE + 0x0001)
/**
 @brief Subnet mask Register address
 */
#define SUBR0			(ETHER_BASE + 0x0005)
/**
 @brief Source MAC Register address
 */
#define SHAR0				(ETHER_BASE + 0x0009)
/**
 @brief Source IP Register address
 */
#define SIPR0				(ETHER_BASE + 0x000F)
/**
 @brief Interrupt Register
 */
#define IR					(ETHER_BASE + 0x0015)
/**
 @brief Interrupt mask register
 */
#define IMR					(ETHER_BASE + 0x0016)
/**
 @brief Timeout register address( 1 is 100us )
 */
#define RTR0				(ETHER_BASE + 0x0017)
/**
 @brief Retry count reigster
 */
#define RCR						(ETHER_BASE + 0x0019)
/**
 @brief Receive memory size reigster
 */
#define RMSR			(ETHER_BASE + 0x001A)
/**
 @brief Transmit memory size reigster
 */
#define TMSR			(ETHER_BASE + 0x001B)
/**
 @brief Authentication type register address in PPPoE mode
 */
#define PATR0					(ETHER_BASE + 0x001C)
//#define PPPALGO (ETHER_BASE + 0x001D)
#define PTIMER (ETHER_BASE + 0x0028)
#define PMAGIC (ETHER_BASE + 0x0029)

/**
 @brief Unreachable IP register address in UDP mode
 */
#define UIPR0				(ETHER_BASE + 0x002A)
/**
 @brief Unreachable Port register address in UDP mode
 */
#define UPORT0			(ETHER_BASE + 0x002E)

/**
 @brief socket register
*/
#define CH_BASE (ETHER_BASE + 0x0400)
/**
 @brief	size of each channel register map
 */
#define CH_SIZE		0x0100
/**
 @brief socket Mode register
 */
#define Sn_MR(ch)		(CH_BASE + ch * CH_SIZE + 0x0000)
/**
 @brief channel Sn_CR register
 */
#define Sn_CR(ch)				(CH_BASE + ch * CH_SIZE + 0x0001)
/**
 @brief channel interrupt register
 */
#define Sn_IR(ch)			(CH_BASE + ch * CH_SIZE + 0x0002)
/**
 @brief channel status register
 */
#define Sn_SR(ch)			(CH_BASE + ch * CH_SIZE + 0x0003)
/**
 @brief source port register
 */
#define Sn_PORT0(ch)		(CH_BASE + ch * CH_SIZE + 0x0004)
/**
 @brief Peer MAC register address
 */
#define Sn_DHAR0(ch)			(CH_BASE + ch * CH_SIZE + 0x0006)
/**
 @brief Peer IP register address
 */
#define Sn_DIPR0(ch)			(CH_BASE + ch * CH_SIZE + 0x000C)
/**
 @brief Peer port register address
 */
#define Sn_DPORT0(ch)		(CH_BASE + ch * CH_SIZE + 0x0010)
/**
 @brief Maximum Segment Size(Sn_MSSR0) register address
 */
#define Sn_MSSR0(ch)					(CH_BASE + ch * CH_SIZE + 0x0012)
/**
 @brief Protocol of IP Header field register in IP raw mode
 */
#define Sn_PROTO(ch)			(CH_BASE + ch * CH_SIZE + 0x0014)

/** 
 @brief IP Type of Service(TOS) Register 
 */
#define Sn_TOS(ch)						(CH_BASE + ch * CH_SIZE + 0x0015)
/**
 @brief IP Time to live(TTL) Register 
 */
#define Sn_TTL(ch)						(CH_BASE + ch * CH_SIZE + 0x0016)

//not support
//#define RX_CH_DMEM_SIZE			(ETHER_BASE + 0x001E)
//#define TX_CH_DMEM_SIZE			(ETHER_BASE + 0x001F)

/**
 @brief Transmit free memory size register
 */
#define Sn_TX_FSR0(ch)	(CH_BASE + ch * CH_SIZE + 0x0020)
/**
 @brief Transmit memory read pointer register address
 */
#define Sn_TX_RD0(ch)			(CH_BASE + ch * CH_SIZE + 0x0022)
/**
 @brief Transmit memory write pointer register address
 */
#define Sn_TX_WR0(ch)			(CH_BASE + ch * CH_SIZE + 0x0024)
/**
 @brief Received data size register
 */
#define Sn_RX_RSR0(ch)	(CH_BASE + ch * CH_SIZE + 0x0026)
/**
 @brief Read point of Receive memory
 */
#define Sn_RX_RD0(ch)			(CH_BASE + ch * CH_SIZE + 0x0028)
/**
 @brief Write point of Receive memory
 */
#define Sn_RX_WR0(ch)			(CH_BASE + ch * CH_SIZE + 0x002A)



/* MODE register values */
#define MR_RST			0x80 /**< reset */
#define MR_PB			0x10 /**< ping block */
#define MR_PPPOE		0x08 /**< enable pppoe */
#define MR_LB  			0x04 /**< little or big endian selector in indirect mode */
#define MR_AI			0x02 /**< auto-increment in indirect mode */
#define MR_IND			0x01 /**< enable indirect mode */

/* IR register values */
#define IR_CONFLICT		0x80 /**< check ip confict */
#define IR_UNREACH		0x40 /**< get the destination unreachable message in UDP sending */
#define IR_PPPoE		0x20 /**< get the PPPoE close message */
#define IR_SOCK(ch)		(0x01 << ch) /**< check socket interrupt */

/* Sn_MR values */
#define Sn_MR_CLOSE		0x00		/**< unused socket */
#define Sn_MR_TCP		0x01		/**< TCP */
#define Sn_MR_UDP		0x02		/**< UDP */
#define Sn_MR_IPRAW		0x03		/**< IP LAYER RAW SOCK */
#define Sn_MR_MACRAW	0x04		/**< MAC LAYER RAW SOCK */
#define Sn_MR_PPPOE		0x05		/**< PPPoE */
#define Sn_MR_ND		0x20		/**< No Delayed Ack(TCP) flag */
#define Sn_MR_MULTI		0x80		/**< support multicating */


/* Sn_CR values */
#define Sn_CR_OPEN		0x01		/**< initialize or open socket */
#define Sn_CR_LISTEN	0x02		/**< wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT	0x04		/**< send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON	0x08		/**< send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE		0x10		/**< close socket */
#define Sn_CR_SEND		0x20		/**< updata txbuf pointer, send data */
#define Sn_CR_SEND_MAC	0x21		/**< send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP	0x22		/**<  send keep alive message */
#define Sn_CR_RECV		0x40		/**< update rxbuf pointer, recv data */

#ifdef __DEF_IINCHIP_PPP__
	#define Sn_CR_PCON				0x23		 
	#define Sn_CR_PDISCON			0x24		 
	#define Sn_CR_PCR				0x25		 
	#define Sn_CR_PCN				0x26		
	#define Sn_CR_PCJ				0x27		
#endif

/* Sn_IR values */
#ifdef __DEF_IINCHIP_PPP__
	#define Sn_IR_PRECV			0x80		
	#define Sn_IR_PFAIL			0x40		
	#define Sn_IR_PNEXT			0x20		
#endif
#define Sn_IR_SEND_OK			0x10		/**< complete sending */
#define Sn_IR_TIMEOUT			0x08		/**< assert timeout */
#define Sn_IR_RECV				0x04		/**< receiving data */
#define Sn_IR_DISCON			0x02		/**< closed socket */
#define Sn_IR_CON				0x01		/**< established connection */

/* Sn_SR values */
#define SOCK_CLOSED				0x00		/**< closed */
#define SOCK_INIT 				0x13		/**< init state */
#define SOCK_LISTEN				0x14		/**< listen state */
#define SOCK_SYNSENT	   		0x15		/**< connection state */
#define SOCK_SYNRECV		   	0x16		/**< connection state */
#define SOCK_ESTABLISHED		0x17		/**< success to connect */
#define SOCK_FIN_WAIT			0x18		/**< closing state */
#define SOCK_CLOSING		   	0x1A		/**< closing state */
#define SOCK_TIME_WAIT			0x1B		/**< closing state */
#define SOCK_CLOSE_WAIT			0x1C		/**< closing state */
#define SOCK_LAST_ACK			0x1D		/**< closing state */
#define SOCK_UDP				0x22		/**< udp socket */
#define SOCK_IPRAW			   	0x32		/**< ip raw mode socket */
#define SOCK_MACRAW			   	0x42		/**< mac raw mode socket */
#define SOCK_PPPOE				0x5F		/**< pppoe socket */

/* IP PROTOCOL */
#define IPPROTO_IP              0           /**< Dummy for IP */
#define IPPROTO_ICMP            1           /**< Control message protocol */
#define IPPROTO_IGMP            2           /**< Internet group management protocol */
#define IPPROTO_GGP             3           /**< Gateway^2 (deprecated) */
#define IPPROTO_TCP             6           /**< TCP */
#define IPPROTO_PUP             12          /**< PUP */
#define IPPROTO_UDP             17          /**< UDP */
#define IPPROTO_IDP             22          /**< XNS idp */
#define IPPROTO_ND              77          /**< UNOFFICIAL net disk protocol */
#define IPPROTO_RAW             255         /**< Raw IP packet */


unsigned char IINCHIP_READ(unsigned int addr);
void IINCHIP_WRITE(unsigned int addr, unsigned char data);
unsigned int wiz_read_buf(unsigned int addr, unsigned char* buf,unsigned int len);
unsigned int wiz_write_buf(unsigned int addr,unsigned char* buf,unsigned int len);

void iinchip_init(void); // reset iinchip
void sysinit(unsigned char tx_size, unsigned char rx_size); // setting tx/rx buf size
unsigned char getISR(unsigned char s);
void putISR(unsigned char s, unsigned char val);
unsigned int getIINCHIP_RxMAX(unsigned char s);
unsigned int getIINCHIP_TxMAX(unsigned char s);
unsigned int getIINCHIP_RxMASK(unsigned char s);
unsigned int getIINCHIP_TxMASK(unsigned char s);
unsigned int getIINCHIP_RxBASE(unsigned char s);
unsigned int getIINCHIP_TxBASE(unsigned char s);
void setGAR(unsigned char * addr); // set gateway address
void setSUBR(unsigned char * addr); // set subnet mask address
void setSHAR(unsigned char * addr); // set local MAC address
void setSIPR(unsigned char * addr); // set local IP address
void setRTR(unsigned int timeout); // set retry duration for data transmission, connection, closing ...
void setRCR(unsigned char retry); // set retry count (above the value, assert timeout interrupt)
void setIMR(unsigned char mask); // set interrupt mask. 
void getGAR(unsigned char * addr);
void getSUBR(unsigned char * addr);
void getSHAR(unsigned char * addr);
void getSIPR(unsigned char * addr);
unsigned char getIR( void );
void setSn_MSS(SOCKET s, unsigned int Sn_MSSR0); // set maximum segment size
void setSn_PROTO(SOCKET s, unsigned char proto); // set IP Protocol value using IP-Raw mode
unsigned char getSn_IR(SOCKET s); // get socket interrupt status
unsigned char getSn_SR(SOCKET s); // get socket status
unsigned int getSn_TX_FSR(SOCKET s); // get socket TX free buf size
unsigned int getSn_RX_RSR(SOCKET s); // get socket RX recv buf size
void setSn_DHAR(SOCKET s, unsigned char * addr);
void setSn_DIPR(SOCKET s, unsigned char * addr);
void setSn_DPORT(SOCKET s, unsigned char * addr);
void getSn_DHAR(SOCKET s, unsigned char * addr);
void getSn_DIPR(SOCKET s, unsigned char * addr);
void getSn_DPORT(SOCKET s, unsigned char * addr);
void setSn_TTL(SOCKET s, unsigned char ttl);
void setMR(unsigned char val);

void send_data_processing(SOCKET s, unsigned char *data, unsigned int len);
void recv_data_processing(SOCKET s, unsigned char *data, unsigned int len);
void read_data(SOCKET s, unsigned char * src, unsigned char * dst, unsigned int len);
void write_data(SOCKET s, unsigned char * src, unsigned char * dst, unsigned int len);

extern unsigned char socket(SOCKET s, unsigned char protocol, unsigned int port, unsigned char flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s); // Close socket
extern unsigned char connect(SOCKET s, unsigned char * addr, unsigned int port); // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s); // disconnect the connection
extern unsigned char listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern unsigned int send(SOCKET s, const unsigned char * buf, unsigned int len); // Send data (TCP)
extern unsigned int recv(SOCKET s, unsigned char * buf, unsigned int len);	// Receive data (TCP)
extern unsigned int sendto(SOCKET s, const unsigned char * buf, unsigned int len, unsigned char * addr, unsigned int port); // Send data (UDP/IP RAW)
extern unsigned int sendto_test(SOCKET s, const unsigned char * buf, unsigned int len, unsigned char * addr, unsigned int port); // Send data (UDP/IP RAW)
extern unsigned int recvfrom(SOCKET s, unsigned char * buf, unsigned int len, unsigned char * addr, unsigned int  *port); // Receive data (UDP/IP RAW)
extern void loopback_tcps(unsigned char ch, unsigned int port_num);
extern int TCP_Connect_Check(SOCKET s);
#endif
