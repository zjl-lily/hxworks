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
	TIMESTRUCT  m_stTime; // 当前时间

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
	void stopDev();        // 清空设备
	void startDev();         //初始化设备

	
//////////////////////////////////////////////////////////////////////////
//  消息处理
	void  AddUDPMsg(UDPMSG_RECV_INFO  stMsg);  // 添加 需要发送的 网络 消息
//////////////////////////////////////////////////////////////////////////

	void  ProcessUDPCmd_WS3(UINT uID, UDPMSG_RECV_INFO*  pUdpMsg) ;
	void  ProcessUDPCmd(); // 处理收到的网络命令
	void  ProcessOneUDPCmd(UDPMSG_RECV_INFO* pUdpcmd); // 处理收到的网络命令


	void  ProcessOneComCmd(MSG_RECV_INFO* pComcmd); // 处理收到的串口命令
	void  ProcessComCmd(); // 处理收到的串口命令 

	TIMESTRUCT  GetCurTime();

	void  UpdateDevState(); // 更新本机的自检状态

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////

};

extern  sysProc    ptrSP; 


#endif // !defined(AFX_SYSPROC_H__8511F01B_B88E_488F_ABDD_FF6610E3F4CE__INCLUDED_)
