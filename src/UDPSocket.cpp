// UDPSocket.cpp: implementation of the UDPSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "../include/UDPSocket.h"
#include <time.h>
#include <sys/time.h>

#include <sys/types.h>  
#include <string.h>  
#include <stdlib.h>  
#include <sys/ioctl.h>  
#include <stdio.h>  
#include <errno.h>  
#include <net/if.h>  
  
struct ethtool_value {  
        u32      cmd;  
        u32      data;  
};  
  
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define INET_ADDR_LEN  16
#define UDPPORT  8000 // 0x5600

#define UDP_DEBUG  0

#if (UDP_DEBUG)
// #ifdef UDP_DEBUG
// 	#if (UDP_DEBUG)
// 		static const char*  pLocalIP = "192.168.1.120";  /*** 本机   192.21.202.8    ***/  
// 	#else
// 		static const char*  pLocalIP = "192.168.1.152";  /*** 本机   192.21.202.8    ***/  
// 	#endif
	static const char*  pLocalIP = "192.168.10.120";  /*** 本机   192.21.202.8    ***/  
	static const char*  pDHIP    = "192.168.10.200";  /*** 导航   192.21.204.3    ***/
	static const char*  pSTIP	 = "192.168.10.110";  /*** 时统   192.21.202.30   ***/
	static const char*  pWS3IP   = "192.168.10.180";  /*** 外设3  192.21.201.1~8  ***/
	static const char*  pWS4IP   = "192.168.10.110";  /*** 外设4  192.21.202.1    ***/
 
	static const char*  pZB1IP   = "224.168.10.101";  /*** 组播2  224.21.200.101  ***/   
	static const char*  pZB2IP   = "224.168.10.107";  /*** 组播1  224.21.200.107  ***/   

#else

	static const char*  pLocalIP = "192.168.3.65"; // 192.21.202.8   
	static const char*  pDHIP    = "192.168.3.110";   /*** 导航  192.21.204.3  ****/
	static const char*  pSTIP	 = "192.168.3.152";  /***时统 192.21.202.30  ***/
	static const char*  pWS3IP   = "192.168.3.200";  /*** 外设3  192.21.201.1~8 *******/
	static const char*  pWS4IP   = "192.168.3.100";  /*** 外设4 192.21.202.1 ***/
 
	static const char*  pZB1IP   = "224.168.3.101";/*** 组播2  224.21.200.101  ****/   
	static const char*  pZB2IP   = "224.168.3.107";/*** 组播1  224.21.200.107  ****/   


// static const char*  pLocalIP = "192.21.202.8"; // 192.21.202.8   

// static const char*  pDHIP    = "192.21.204.3";   /*** 导航   192.21.204.3    ****/
// static const char*  pSTIP    = "192.21.202.30";  /*** 时统   192.21.202.30   ****/

// static const char*  pWS3IP   = "192.168.1.59";   /*** 外设3  192.21.201.1~8  ****/
// static const char*  pWS4IP   = "192.21.202.1";   /*** 外设4  192.21.202.1    ****/
//  
// static const char*  pZB1IP   = "224.21.200.101"; /*** 组播1  224.21.200.101  ****/   
// static const char*  pZB2IP   = "224.21.200.107"; /*** 组播2  224.21.200.107  ****/   

#endif


//////////////////////////////////////////////////////////////////////////

UDPSocket::UDPSocket()
{
	m_uPort = UDPPORT;
	InitUDPMsgQueue(&m_stRMsgList);
	InitUDPMsgQueue(&m_stSMsgList);
	InitUART(&m_stUart);
	m_pRecvMsg = NULL;
	m_sockFD = -2;
	m_uPort = UDPPORT;
	m_lRongYu = 0;
	m_lRecvCount = 0;
	m_UDPSID = 0;
	m_state = 0x00;
	m_Rongyu = 0;
	m_uLocalAddr = inet_addr(pLocalIP);
    m_uDHAddr = inet_addr(pDHIP);
    m_uSTAddr = inet_addr(pSTIP);
    m_uWS3Addr = inet_addr(pWS3IP);
    m_uWS4Addr = inet_addr(pWS4IP);
	m_uZB1Addr = inet_addr(pZB1IP);
	m_uZB2Addr = inet_addr(pZB2IP);
}

UDPSocket::~UDPSocket()
{
	CloseSocket();
}  

const char*  UDPSocket::GetUserIP(int uT)
{
	const char *pIP = NULL;
	switch(uT)
	{
	case UDP_USER_DH:
		pIP = pDHIP;
		break;
	case UDP_USER_ST:
		pIP = pSTIP;
		break;
	case UDP_USER_WS3:
		pIP = pWS3IP;
		break;
	case UDP_USER_WS4:
		pIP = pWS4IP;
		break;
	case UDP_USER_ZB1:
		pIP = pZB1IP;
		break;
	case UDP_USER_ZB2:
		pIP = pZB2IP;
		break;
	default:
		pIP = pWS3IP;
		break;
	}
	return pIP;
}

BOOL   UDPSocket::OpenSocket()
{
	if (m_bConn)
	{//已经打开
		return TRUE;
	}
 
	printf("LIP: %s,  %x; prot = %ld \n",pLocalIP, m_uLocalAddr, m_uPort);
//	printf("DH:  %s,  %x; \nST:  %s,  %x;\nWS3: %s,  %x;\nWS4: %s,  %x;\nZB1: %s,  %x;\nZB2: %s,  %x;\n",
//		pDHIP, m_uDHAddr, pSTIP, m_uSTAddr, pWS3IP, m_uWS3Addr,
//		pWS4IP, m_uWS4Addr, pZB1IP, m_uZB1Addr, pZB2IP, m_uZB2Addr);

	int  ret;
	m_sockFD =  socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
 	if ( m_sockFD < 0)
	{
		perror("OpenSocket(): socket创建失败\n");
		m_state = 0x80;
		return FALSE;
	} 
 	int len =  sizeof(struct sockaddr_in);
	memset(&m_addrIN, 0x00, len);
    
	 
	m_addrIN.sin_family = AF_INET;
	m_addrIN.sin_port = htons(m_uPort);//注意端口号应该与发送端目的地址的端口号一致
  	m_addrIN.sin_addr.s_addr =  inet_addr(pLocalIP);
 	m_addrIN.sin_addr.s_addr =  htonl(INADDR_ANY);
   
	// 绑定socket绑定socket
	ret = bind(m_sockFD,(struct sockaddr*)&m_addrIN, len);
	if ( ret < 0 )
	{
		close(m_sockFD);
		perror("OpenSocket(): bind\n"); 
		m_state = 0x80;
		return FALSE;
	}
	else 
	{
        printf("net connect OK !!!!!!!!!!\n");
		m_bConn = TRUE;
/*
		ret = setsockopt(m_sockFD,IPPROTO_IP,IP_MULTICAST_IF,(char*)&m_addrIN,sizeof(m_addrIN));
		if ( ret < 0 )
		{
			close(m_sockFD);
	    	perror("OpenSocket(): IP_MULTICAST_IF   \n");  
			m_state = 0x80;
			return FALSE;
		}
		// 用于接收多播所设置，把套接字加入一个多播组
		struct ip_mreq  mcast;
		memset(&mcast,0x00,sizeof(mcast));
 
		mcast.imr_multiaddr.s_addr = inet_addr(pZB2IP);
		mcast.imr_interface.s_addr = inet_addr(pLocalIP);
		ret = setsockopt(m_sockFD,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast));
		if ( ret < 0 )
		{
			close(m_sockFD);
	    	perror("OpenSocket(): setsockopt 11 \n");  
			m_state = 0x80;
			return FALSE;
		}
		memset(&mcast,0x00,sizeof(mcast));
		mcast.imr_multiaddr.s_addr = inet_addr(pZB1IP);
		mcast.imr_interface.s_addr = inet_addr(pLocalIP);;
		ret = setsockopt(m_sockFD,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast));
		if ( ret < 0 )
		{
			close(m_sockFD);
	    	perror("OpenSocket(): setsockopt 22 \n");  
			m_state = 0x80;
			return FALSE; 
		}
		m_bConn = TRUE;
        // 防止接收到自己发送的组播报文。
		bool bloop = false;
		ret = setsockopt(m_sockFD,IPPROTO_IP,IP_MULTICAST_LOOP, &bloop,sizeof(bool));
*/
    }

 	return m_bConn;
}
BOOL  UDPSocket::CloseSocket()
{
	if (m_bConn && m_sockFD > 0)
	{
		close(m_sockFD);
		m_sockFD = -1;
		m_bConn = FALSE;
	}

	return m_bConn;
}
int UDPSocket::FetchOneMsg(BYTE* pinfo, int len, UDP_USER_TYPE  userID)
{ // 从接收到的数据中 分解一个完整报文
	if (!pinfo || len < 1)
	{
		return 0;
	}
	int nmsg = 0;
	UDPMSG_RECV_INFO*  pMsg = NULL;
	for (int i = 0; i < len; i++)
	{
 		// 一个完整报文
		if (!m_pRecvMsg || m_pRecvMsg->state == CMSG_STATE_CHECK)
		{
			pMsg =  GetOneMsg(COM_RECV, CMSG_STATE_IDLE);
			if (!pMsg)
			{
				m_pRecvMsg = &m_stRMsgList.message[0];
				InitUDPMsgInfo(m_pRecvMsg);
			}
			else
				m_pRecvMsg = pMsg;
		}
		nmsg += RecvOneUDPFrame(pinfo[i],m_pRecvMsg);
		if (m_pRecvMsg && m_pRecvMsg->state == CMSG_STATE_CHECK)
		{
			m_pRecvMsg->userID = userID;
 			HexToString(m_pRecvMsg->buff, m_pRecvMsg->len);
 			m_pRecvMsg = NULL;
		}
	}
	return nmsg;
}
int   UDPSocket::UDPRecv() // 从网口接收
{
	u32  uPort = m_uPort;
	char ipbuff[16];
	struct sockaddr_in  addrRecv;
 	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&addrRecv, 0x00, addrlen);

	addrRecv.sin_family = AF_INET;
	addrRecv.sin_port = htons(uPort);//注意端口号应该与发送端目的地址的端口号一致
  	struct timezone tz_cur; 
	struct tm *ptm;
	struct timeval  tv_cur;
 	time_t  tp;

	struct sockaddr  *paddr = (struct sockaddr*)&addrRecv;
	int  recvLen = 0;
	int  nsus = 1;
	UDP_USER_TYPE  userID = UDP_USER_ST;
	char peerIP[20];
	float fradio = 0;
    while(m_bConn)
	{
 		addrRecv.sin_addr.s_addr =  htonl(INADDR_ANY);
		recvLen = recvfrom(m_sockFD, m_stUart.rx_buf, UART_BUFF_SIZE, 0, paddr, &addrlen);
	    if (recvLen > 0 && recvLen < UART_BUFF_SIZE)
	    {
			DebugPrintf(__FILE__, __FUNCTION__, __LINE__); 
			m_stUart.rx_buf[recvLen] = '\0';
			memset(peerIP, '\0', 20);
			sprintf(peerIP,"%s", inet_ntop(AF_INET,	&addrRecv.sin_addr, ipbuff, 16)); 
            AddRecvInfo(m_stUart.rx_buf, recvLen, userID);
        } 
	}
	return 0;
}
void   UDPSocket::shutdownUDP()
{
	if (m_bConn)
	{
		m_bConn = FALSE;
		shutdown(m_sockFD, 2);
		m_sockFD = -1;
	}
}
void   UDPSocket::check_netstate()
{
    return;
   struct ethtool_value edata;  
    int fd = -1, err = 0;  
    struct ifreq ifr;  
  
    memset(&ifr, 0, sizeof(ifr));  
    strcpy(ifr.ifr_name, "eth1");  

    edata.cmd = 0x0000000a;  
    ifr.ifr_data = (caddr_t)&edata;  
    err = ioctl(m_sockFD, 0x8946, &ifr);  
    if (err == 0) 
	{  
 		if(edata.data==1)
		{
			g_stDevState.ZJstate.D4 = 1;
// 			printf("%s link states is up\n", ifr.ifr_name);
		}
		else
		{
			g_stDevState.ZJstate.D4 = 0;
// 			printf("%s link states is down\n", ifr.ifr_name);

		}
    } 

	memset(&ifr, 0, sizeof(ifr));  
    strcpy(ifr.ifr_name, "eth1");  
 
    edata.cmd = 0x0000000a;  
    ifr.ifr_data = (caddr_t)&edata;  
    err = ioctl(m_sockFD, 0x8946, &ifr);  

    if (err == 0) 
	{  
 		if(edata.data==1)
		{
			g_stDevState.ZJstate.D5 = 1;
		}
		else
		{
			g_stDevState.ZJstate.D5 = 0; 
		}
    } 

}
bool  UDPSocket::AddRecvInfo(BYTE* pinfo, int len, UDP_USER_TYPE userID) // 将需要发送的报文放到  发送报文列表
{
	UDPMSG_RECV_INFO*  pMsg = GetOneMsg(COM_RECV, CMSG_STATE_IDLE);
	if (!pinfo || len > MAX_MSG_LEN || len < 1)
	{
		return false;
	}
	else if(!pMsg)
	{
		pMsg = &m_stRMsgList.message[0];
	}
	else;

	pMsg->len = len;
	pMsg->cnt = len;
	pMsg->userID = userID;
	memcpy(pMsg->buff, pinfo, len);
	pMsg->state = CMSG_STATE_CHECK;
	return true;
}
int  UDPSocket::UDPSend(char* pdata, int len, struct sockaddr_in  addrcli)
{ 
 	int addrlen = sizeof(struct sockaddr_in); 	 
	struct sockaddr*  paddr = (struct sockaddr*)&addrcli;

 	int  sendLen = sendto(m_sockFD, pdata, len, 0, paddr, addrlen);
	return sendLen;
}
int  UDPSocket::UDPSendData(char* pdata, int len, UDP_USER_TYPE userID)
{  
 	if (m_sockFD < 0 || !m_bConn)
	{
		return 0;
	}
	struct sockaddr_in  addrcli;
	struct sockaddr*  paddr = (struct sockaddr*)&addrcli;
 	int addrlen = sizeof(struct sockaddr_in); 	 

	memset(&addrcli, 0x00, sizeof(addrcli));
	addrcli.sin_family = AF_INET;
	addrcli.sin_port = htons(m_uPort);//注意端口号应该与发送端目的地址的端口号一致
 	
	int n = 0; 
    const char *pIP = NULL;
    
 
	switch(userID)
	{// 设置 接收方 IP
		case UDP_USER_DH:
			pIP =  pDHIP;
			break;
		case UDP_USER_ST:
			pIP =  pSTIP;
			break;
		case UDP_USER_WS3:
			pIP =  pWS3IP;
			break;
		case UDP_USER_WS4:
			pIP =  pWS4IP;
			break;
		case UDP_USER_ZB1:
			pIP =  pZB1IP;
			break;
		case UDP_USER_ZB2:
			pIP =  pZB2IP;
			break;
		default:
			pIP =  pDHIP;
			break;
	}
	addrcli.sin_addr.s_addr =   inet_addr(pIP); 
	int msglen = sendto(m_sockFD, pdata, len, 0, (struct sockaddr*)&addrcli, addrlen);
 
  	return msglen;
}
int  UDPSocket::SendMsg() // 发送报文
{
//////////////////////////////////////////////////////////////////////////
	if (m_sockFD < 0 || !m_bConn)
	{
		return 0;
	}
	struct sockaddr_in  addrcli;
	struct sockaddr*  paddr = (struct sockaddr*)&addrcli;
 	int addrlen = sizeof(struct sockaddr_in); 	 

	memset(&addrcli, 0x00, sizeof(addrcli));
	addrcli.sin_family = AF_INET;
	addrcli.sin_port = htons(m_uPort);//注意端口号应该与发送端目的地址的端口号一致
 	
	int n = 0;
	int len = 0;
	UDPMSG_RECV_INFO*  pMsg = NULL;
    const char *pIP = NULL;
    
	for (n = 0; n < MAX_MSG_NUM; n++)
	{
		pMsg = &m_stSMsgList.message[n];
		if (pMsg->state == CMSG_STATE_CHECK)
		{ 

			switch(pMsg->userID)
			{// 设置 接收方 IP
				case UDP_USER_DH:
					pIP =  pDHIP;
					break;
				case UDP_USER_ST:
					pIP =  pSTIP;
					break;
				case UDP_USER_WS3:
					pIP =  pWS3IP;
					break;
				case UDP_USER_WS4:
					pIP =  pWS4IP;
					break;
				case UDP_USER_ZB1:
					pIP =  pZB1IP;
					break;
				case UDP_USER_ZB2:
					pIP =  pZB2IP;
					break;
				default:
					pIP =  pDHIP;
					break;
			}
			addrcli.sin_addr.s_addr =   inet_addr(pIP);
			
// 			len = sendto(m_sockFD, pMsg->buff, pMsg->len, 0, paddr, addrlen);
			len = sendto(m_sockFD, pMsg->buff, pMsg->len, 0, (struct sockaddr*)&addrcli, addrlen);
//   		len = UDPSend(pMsg->buff, pMsg->len, addrcli);
// 			if (len > 0)
// 			{
// 				printf("UDP send data to %s,len = %d \n", pIP, len);
// 			}
// 			else
// 			{
// 				printf("UDP send failed!  ip = %s; len=%d \n", pIP, len);
// 			}
			InitUDPMsgInfo(pMsg);
		} 
	} 
	return 1;
}
 bool  UDPSocket::AddSendMsg(UDPMSG_RECV_INFO msg)// 将需要发送的报文放到  发送报文列表
 {
	 if (msg.state != CMSG_STATE_CHECK  || msg.len > MAX_MSG_LEN || msg.len < 1)
	 {
		 return false;
	 }
	 UDPMSG_RECV_INFO* pMsg = GetOneMsg(COM_SEND, CMSG_STATE_IDLE);
	 if (!pMsg)
	 {
		 pMsg = &m_stSMsgList.message[0];
	 }
	 pMsg->len = msg.len; 
	 pMsg->cnt = msg.cnt; 
	 pMsg->state = msg.state;
	 pMsg->userID = msg.userID;
	 memcpy(pMsg->buff, msg.buff, msg.len);

	 return true;
 }

bool  UDPSocket::AddSendInfo(BYTE* pinfo, int len, UDP_USER_TYPE userID) // 将需要发送的报文放到  发送报文列表
{
	UDPMSG_RECV_INFO*  pMsg = GetOneMsg(COM_SEND, CMSG_STATE_IDLE);
	if (!pinfo || len > MAX_MSG_LEN || len < 1)
	{
		return false;
	}
	else if(!pMsg)
	{
		pMsg = &m_stSMsgList.message[0];
	}
	else;

	pMsg->len = len;
	pMsg->cnt = len;
	pMsg->userID = userID;
	memcpy(pMsg->buff, pinfo, len);
	pMsg->state = CMSG_STATE_CHECK;
	return true;
} 
UDPMSG_RECV_INFO*  UDPSocket::GetOneMsg(int type,int state) //  从收到的报文中提取一个完整报文
{
	int n = 0;
	UDPMSG_INFO_LIST*  pList = NULL;
	UDPMSG_RECV_INFO*  pMsg = NULL;
	if (type == COM_RECV)
	{
		 pList = &m_stRMsgList;
	}
	else
		 pList = &m_stSMsgList;

	for (n = 0; n < MAX_MSG_NUM; n++)
	{
		pMsg = &pList->message[n];
		if (pMsg->state == state)
		{
 			break;
		} 
	}
	if (n >= MAX_MSG_NUM)
	{
		pMsg = NULL;
	}
	return pMsg;
} 
BYTE  UDPSocket::getUDPSID()
{
	BYTE  bID = m_UDPSID;
	m_UDPSID++;
	return bID;
}
