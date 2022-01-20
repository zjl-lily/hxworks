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
    UDPMSG_RECV_INFO  *m_pRecvMsg; // ���ڽ��յ�
    BOOL    m_bConn; // ���ӣ�������
	int     m_sockFD;
	u32     m_uPort;  // �˿�

	BYTE    m_state;  //

	u32     m_uLocalAddr;
	u32     m_uDHAddr;
	u32     m_uSTAddr;
	u32     m_uWS3Addr;
	u32     m_uWS4Addr; 
	u32     m_uZB1Addr; 
	u32     m_uZB2Addr; 
 
	struct sockaddr_in  m_addrIN;
 
	u8    m_Rongyu;  // 0=�ޣ�1=���෢�Ͳ��ԣ�2=������ղ���
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
	int   UDPSend(char* pdata, int len, struct sockaddr_in  addrcli); // ���͵�����
	int   UDPRecv(); // �����ڽ���
	
	int   SendMsg(); // ���ͱ���

 	
	bool  AddSendInfo(BYTE* pinfo, int len, UDP_USER_TYPE userID); // ����Ҫ���͵ı��ķŵ�  ���ͱ����б�
	bool  AddRecvInfo(BYTE* pinfo, int len, UDP_USER_TYPE userID); // ����Ҫ���͵ı��ķŵ�  ���ͱ����б�
	bool  AddSendMsg(UDPMSG_RECV_INFO msg); // ����Ҫ���͵ı��ķŵ�  ���ͱ����б�
	UDPMSG_RECV_INFO*  GetOneMsg(int type,int state); //  ���յ��ı�������ȡһ����������

	int FetchOneMsg(BYTE* pinfo, int len, UDP_USER_TYPE  userID);

	const  char*  GetUserIP(int uT);
	
	BYTE  getUDPSID();
private: 

	BYTE  m_UDPSID; // �������

};

#endif // !defined(AFX_UDPSOCKET_H__EEE63353_F994_4B05_B870_2C9ECFCC1B73__INCLUDED_)
