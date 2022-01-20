// UDPSocket.h: interface for the UDPSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UDPSOCKET_H__EEE63353_F994_4B05_B870_2C9ECFCC1B73__INCLUDED_)
#define AFX_UDPSOCKET_H__EEE63353_F994_4B05_B870_2C9ECFCC1B73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/uart.h"

#include <string.h>
#include <unistd.h>
 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h> 

using namespace std;

class UDPSocket  
{
public:
	UARTType  m_stUart;
	UDPMSG_INFO_LIST  m_stSMsgList;//
    UDPMSG_INFO_LIST  m_stRMsgList;//
    UDPMSG_RECV_INFO  *m_pRecvMsg; // 正在接收的
    BOOL    m_bConn; // 连接（创建）
	int     m_sockFD;
	u32     m_uPort;  // 端口

	BYTE    m_state;  //

	u32     m_uLocalAddr;
	u32     m_uDHAddr;
	u32     m_uSTAddr;
	u32     m_uWS3Addr;
	u32     m_uWS4Addr; 
	u32     m_uZB1Addr; 
	u32     m_uZB2Addr; 
 
	struct sockaddr_in  m_addrIN;
 
	u8    m_Rongyu;  // 0=无，1=冗余发送测试，2=冗余接收测试
	long  m_lRongYu;
	long  m_lRecvCount;
//////////////////////////////////////////////////////////////////////////
	UDPSocket();
	virtual ~UDPSocket();

//////////////////////////////////////////////////////////////////////////
// function:
 	BOOL   OpenSocket();
 	BOOL   CloseSocket();
	void   shutdownUDP();

	void   check_netstate();

	int   UDPSendData(char* pdata, int len, UDP_USER_TYPE userID);
	int   UDPSend(char* pdata, int len, struct sockaddr_in  addrcli); // 发送到网口
	int   UDPRecv(); // 从网口接收
	
	int   SendMsg(); // 发送报文

 	
	bool  AddSendInfo(BYTE* pinfo, int len, UDP_USER_TYPE userID); // 将需要发送的报文放到  发送报文列表
	bool  AddRecvInfo(BYTE* pinfo, int len, UDP_USER_TYPE userID); // 将需要发送的报文放到  发送报文列表
	bool  AddSendMsg(UDPMSG_RECV_INFO msg); // 将需要发送的报文放到  发送报文列表
	UDPMSG_RECV_INFO*  GetOneMsg(int type,int state); //  从收到的报文中提取一个完整报文

	int FetchOneMsg(BYTE* pinfo, int len, UDP_USER_TYPE  userID);

	const  char*  GetUserIP(int uT);
	
	BYTE  getUDPSID();
private: 

	BYTE  m_UDPSID; // 网络序号

};

#endif // !defined(AFX_UDPSOCKET_H__EEE63353_F994_4B05_B870_2C9ECFCC1B73__INCLUDED_)
