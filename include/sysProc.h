// sysProc.h: interface for the sysProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYSPROC_H__8511F01B_B88E_488F_ABDD_FF6610E3F4CE__INCLUDED_)
#define AFX_SYSPROC_H__8511F01B_B88E_488F_ABDD_FF6610E3F4CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>     
#include <unistd.h>    
#include <sys/types.h>     
#include <sys/stat.h>       
#include <pthread.h>
#include <fcntl.h>    
#include <stdio.h>
#include <errno.h>
#include <string.h>


#include "../include/SerialPort.h"
#include "../include/UDPSocket.h"
#include "../include/NetMsg.h"
#include "../include/comMsg.h"
#include "../include/ThreadProc.h"




class sysProc  
{
public:
	TIMESTRUCT  m_stTime; // ��ǰʱ��

	SerialPort*   m_pSPlist[COMM_NUM];
	UDPSocket*    m_pUDPSock;
	int   m_comNum;
	bool     m_bRun;

   
	ThreadProc   m_ThProc;


	UDPMSG_RECV_INFO  m_stUDPMsg; 
	MSG_RECV_INFO     m_stComMsg;

public:
	sysProc();
	virtual ~sysProc();

//////////////////////////////////////////////////////////////////////////

	void initThread();
	void stopDev();        // ����豸
	void startDev();         //��ʼ���豸

	
//////////////////////////////////////////////////////////////////////////
//  ��Ϣ����
	void  AddUDPMsg(UDPMSG_RECV_INFO  stMsg);  // ��� ��Ҫ���͵� ���� ��Ϣ
//////////////////////////////////////////////////////////////////////////

	void  ProcessUDPCmd_WS3(UINT uID, UDPMSG_RECV_INFO*  pUdpMsg) ;
	void  ProcessUDPCmd(); // �����յ�����������
	void  ProcessOneUDPCmd(UDPMSG_RECV_INFO* pUdpcmd); // �����յ�����������


	void  ProcessOneComCmd(MSG_RECV_INFO* pComcmd); // �����յ��Ĵ�������
	void  ProcessComCmd(); // �����յ��Ĵ������� 

	TIMESTRUCT  GetCurTime();

	void  UpdateDevState(); // ���±������Լ�״̬

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

};

extern  sysProc    ptrSP; 


#endif // !defined(AFX_SYSPROC_H__8511F01B_B88E_488F_ABDD_FF6610E3F4CE__INCLUDED_)
