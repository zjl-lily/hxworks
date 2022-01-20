// sysProc.cpp: implementation of the sysProc class.
//
//////////////////////////////////////////////////////////////////////

#include "../include/sysProc.h"
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include "../include/lxgpio.h"
 
sysProc      ptrSP; 
  
//////////////////////////////////////////////////////////////////////////
sysProc::sysProc()
{
	initThread();
	g_stDevState.DevState = 0x00;
	BYTE*  pb = (BYTE*)&g_stDevState.ZJstate;
	*pb = 0x00;
	m_bRun = false;
    m_pUDPSock = NULL;
}

sysProc::~sysProc()
{ 
	stopDev();
}

void sysProc::stopDev()// 清空设备
{
	m_ThProc.stopThread();
 	m_bRun = false;

	int i = 0;
	for (i = 0; i < COMM_NUM; i++)
	{
		if (m_pSPlist[i] != NULL)
		{
			m_pSPlist[i]->ClosePort();
			delete m_pSPlist[i];
			m_pSPlist[i] = NULL;
		}
	}
	if (m_pUDPSock != NULL)
	{
		m_pUDPSock->CloseSocket();
		delete m_pUDPSock;
		m_pUDPSock = NULL;
	}
	m_comNum = 0; 
}
void sysProc::startDev()//初始化串口设备
{
	initThread();
	m_bRun = true;


	int n = 0, m = 0;
	ComPara  stPara;
	bool  flag = false;
	/*
	for(n = 0; n < COMM_NUM; n++)
	{
		stPara.ComPort = n;
		m_pSPlist[n] = new SerialPort(n);
		flag = m_pSPlist[n]->OpenPort(stPara); 
		switch(n)
		{
		case 0:  // 串口1
			g_stDevState.ZJstate.D0 = flag;
			break;
		case 1:  // 串口1
			g_stDevState.ZJstate.D1 = flag;
			break;
		case 2:  // 串口1
			g_stDevState.ZJstate.D2 = flag;
			break; 
		default:
			break;			
		}
 		m_comNum++;
	}
	*/

	//////////////////////////////////////////////////////////////////////////
// 初始化网络
    m_pUDPSock = new UDPSocket();
	m_pUDPSock->OpenSocket(); 
	//////////////////////////////////////////////////////////////////////////

	m_ThProc.startThread();

}
void sysProc::initThread()
{
	m_pUDPSock = NULL;
	m_comNum = 0; 

 
	for (int i = 0; i < COMM_NUM; i++)
	{
		m_pSPlist[i] = NULL;
	}

	m_pUDPSock = NULL;
}
 
TIMESTRUCT  sysProc::GetCurTime()
{
	struct tm *ptm;
	struct timeval  tv;
	struct timezone tz;
	time_t  tp;
	time(&tp);
	ptm = localtime(&tp);
	gettimeofday(&tv, &tz);

// 	printf("GetCurTime: %04d-%02d-%02d %02d:%02d:%02d\n", (ptm->tm_year+1900),
// 		ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
// 
// 	printf("tv_sec: %ld\n", tv.tv_sec);
// 	printf("tv_usec: %ld\n", tv.tv_usec); 


	m_stTime.bIsValid = TRUE;
	m_stTime.nYear = ptm->tm_year + 1900;
	m_stTime.byMonth = ptm->tm_mon;
	m_stTime.byDay = ptm->tm_mday;
	m_stTime.byHour = ptm->tm_hour;
	m_stTime.byMinutes = ptm->tm_min;
	m_stTime.bySecond = ptm->tm_sec;

	return m_stTime;
}

void  sysProc::UpdateDevState()// 更新本机的自检状态
{
	int i = 0;
	SerialPort*  pSp = NULL;
	for (i = 0;i < COMM_NUM; i++)
	{
		pSp = m_pSPlist[i];
		switch(i)
		{
		case COMM_VS:
			g_stDevState.ZJstate.D0 = pSp->m_state;
			break;
		case COMM_HS:
			g_stDevState.ZJstate.D1 = pSp->m_state;
			break;
		case COMM_AS:
			g_stDevState.ZJstate.D2 = pSp->m_state;
			break; 
		default:
			break;
		}
	}
	g_stDevState.ZJstate.D4 = m_pUDPSock->m_state;

}
//////////////////////////////////////////////////////////////////////////
// 消息处理
void  sysProc::AddUDPMsg(UDPMSG_RECV_INFO  stMsg)  // 添加 需要发送的 网络 消息
{
	m_pUDPSock->AddSendMsg(stMsg);
} 
 
void  sysProc::ProcessComCmd() // 处理收到的串口命令
{
	int i = 0;
 
} 



void  sysProc::ProcessOneComCmd(MSG_RECV_INFO* pComcmd) // 处理收到的串口命令
{
 	MPACKET  udpPacket; 
	UINT     index = 0; 
    BYTE bVLS = 0, bHLS = 0; 
	BYTE*    pBuff = pComcmd->buff; 

  	printf("ProcessOneComCmd: %s:%d, msglen = %d \n", __FILE__, __LINE__, pComcmd->len);
  	HexToString(pBuff, pComcmd->len); 

 // 导航信息报文
	MSG_RECV_INFO  stMsg;
	BYTE uID = 0;
	BYTE  n = 0;  
 	BYTE  *pMsgData = pBuff; 
// 	printf("%s:%d, ProcessUDPCmd:  packetlen = %d; \n", __FILE__, __LINE__,  pUdpcmd->len - index); 
	index = 0;
	uID = pMsgData[1];
    udpPacket.mid = ntohs(*(short*)(pMsgData));        index += 2;
	udpPacket.length = ntohs(*(short*)(pMsgData + index));    index += 2; 
	udpPacket.frameID =  ntohs(*(short*)(pMsgData + index));    index += 2; 
	udpPacket.packetID =  ntohs(*(short*)(pMsgData + index));    index += 2;  

 	udpPacket.msg.bak1 =  ntohl(*(UINT32*)(pMsgData + index));    index += 4; 
 	udpPacket.msg.bak2 =  ntohl(*(UINT32*)(pMsgData + index));    index += 4; 
 	udpPacket.msg.flag =  ntohs(*(short*)(pMsgData + index));    index += 2; 

 	udpPacket.msg.reset =  ntohs(*(short*)(pMsgData + index));    index += 2; 
 	udpPacket.msg.ctrl =  ntohl(*(UINT32*)(pMsgData + index));    index += 4; 
 
 	printf("COMRECV: %s:%d: mid = 0x%x, length = 0x%x, flag = 0x%x, reset=%d, ctrl=%d \n", __FILE__, __LINE__,  
		 udpPacket.mid, udpPacket.length, udpPacket.msg.flag, udpPacket.msg.reset, udpPacket.msg.ctrl);
 	HexToString(pMsgData, udpPacket.length);

	if(uID == NETCMD_EMTR)
	{
		if(udpPacket.msg.ctrl)
			uID = NETCMD_RESET;
	}

 	switch(uID)
	{
		case NETCMD_EMTR:
			g_setdevState.uEmtr = udpPacket.msg.flag;
		   
			if((g_setdevState.uEmtr&0x02)>>1)
			{// VLS=1  bit1
				bVLS = 1;
				gpio_set(GPIO_VLS, gpio_level_high);
			}
			else
			{
				bVLS = 0;
				gpio_set(GPIO_VLS, gpio_level_low);
			}
 
			if((g_setdevState.uEmtr&0x01))
			{// HLS=1   bit0
				bHLS = 1;
				gpio_set(GPIO_HLS, gpio_level_high);
			}
			else
			{
				bHLS = 0;
				gpio_set(GPIO_HLS, gpio_level_low);
			}
 
			if( bHLS && bVLS)
			{// ALS=1
				gpio_set(GPIO_WLS, gpio_level_high);
			}
			else
				gpio_set(GPIO_WLS, gpio_level_low); 
		  
			InitMsgInfo(&stMsg); 
		    printf("%s:%d: NETCMD_EMTR, mid = 0x%02X; flag = 0x%02X \n", __FILE__, __LINE__,  
				udpPacket.mid,g_setdevState.uEmtr);
 			break;
		case NETCMD_RESET:  // 报文长度：4+4+14=22 			
			g_setdevState.uReset = udpPacket.msg.reset;
		 	if(g_setdevState.uReset)
			{// ALS=1
				gpio_set(GPIO_RESET, gpio_level_high);
			}
			else
			    gpio_set(GPIO_RESET, gpio_level_low); 

		    printf("%s:%d: NETCMD_RESET, mid = 0x%02X; reset=0x%02X; \n", __FILE__, __LINE__, 
				 udpPacket.mid,udpPacket.msg.reset);
			break; 
		default:
			break;
	}

} 
 
void  sysProc::ProcessUDPCmd_WS3(UINT uID, UDPMSG_RECV_INFO*  pUdpMsg) 
{
 
}
void  sysProc::ProcessUDPCmd() // 处理收到的网络命令
{
 	UDPMSG_RECV_INFO*pMsg = m_pUDPSock->GetOneMsg(COM_RECV, CMSG_STATE_CHECK);
	if(pMsg && pMsg->state == CMSG_STATE_CHECK)
	{
		ProcessOneUDPCmd(pMsg);
		if (pMsg && pMsg->len > 0)
		{ 
			InitUDPMsgInfo(pMsg);
		}
	}
}
void  sysProc::ProcessOneUDPCmd(UDPMSG_RECV_INFO* pUdpcmd) // 处理收到的网络命令
{
//	UINT uLen = 0;
    BYTE     msgID = 0x00;
	MPACKET  udpPacket; 
	UINT     index = 0;
	UINT     uHeadLen = 0;
	BYTE*    pBuff = pUdpcmd->buff; 
    BYTE bVLS = 0, bHLS = 0; 

  	printf("ProcessUDPCmd: %s:%d, msglen = %d \n", __FILE__, __LINE__, pUdpcmd->len);
  	HexToString(pBuff, pUdpcmd->len); 

 // 导航信息报文
	MSG_RECV_INFO  stMsg;
	UINT uID = 0;
	BYTE  n = 0;
	index = UDPMSG_HEAD_LEN; // 
	uHeadLen = index;
	if(pBuff[0] == 0x3F && pBuff[1] == 0x15)
		index = 0;
	else
	{
		printf("ProcessUDPCmd: %s:%d   Msg is Error!\n", __FILE__, __LINE__);
		return ;
	}
 	BYTE  *pMsgData = pBuff + index; 
// 	printf("%s:%d, ProcessUDPCmd:  packetlen = %d; \n", __FILE__, __LINE__,  pUdpcmd->len - index); 
	index = 0;
	msgID = pMsgData[1];
    udpPacket.mid = ntohs(*(short*)(pMsgData));        index += 2;
	udpPacket.length = ntohs(*(short*)(pMsgData + index));    index += 2; 
	udpPacket.frameID =  ntohs(*(short*)(pMsgData + index));    index += 2; 
	udpPacket.packetID =  ntohs(*(short*)(pMsgData + index));    index += 2;  

 	udpPacket.msg.bak1 =  ntohl(*(UINT32*)(pMsgData + index));    index += 4; 
 	udpPacket.msg.bak2 =  ntohl(*(UINT32*)(pMsgData + index));    index += 4; 
 	udpPacket.msg.flag =  ntohs(*(short*)(pMsgData + index));    index += 2; 

 	udpPacket.msg.reset =  ntohs(*(short*)(pMsgData + index));    index += 2; 
 	udpPacket.msg.ctrl =  ntohl(*(UINT32*)(pMsgData + index));    index += 4; 
 
 	printf("mid = 0x%02X, msgID = 0x%02X; length = 0x%02x;\nflag = 0x%02X; reset = 0x%02X; ctrl=0x%02X\n", 
		 udpPacket.mid, msgID, udpPacket.length, udpPacket.msg.flag, udpPacket.msg.reset, udpPacket.msg.ctrl);
 	HexToString(pMsgData, udpPacket.length);
    if(udpPacket.mid == 0x3F15)
	{
		if(udpPacket.msg.ctrl == 1)
			msgID = NETCMD_RESET;
	} 
 	switch(msgID)
	{
		case NETCMD_EMTR:
			g_setdevState.uEmtr = udpPacket.msg.flag;
		   
			if((g_setdevState.uEmtr&0x02)>>1)
			{// VLS=1  bit1
				bVLS = 1;
				gpio_set(GPIO_VLS, gpio_level_high);
			}
			else
			{
				bVLS = 0;
				gpio_set(GPIO_VLS, gpio_level_low);
			}
 
			if((g_setdevState.uEmtr&0x01))
			{// HLS=1   bit0
				bHLS = 1;
				gpio_set(GPIO_HLS, gpio_level_high);
			}
			else
			{
				bHLS = 0;
				gpio_set(GPIO_HLS, gpio_level_low);
			}
 
			if( bHLS && bVLS)
			{// ALS=1
				gpio_set(GPIO_WLS, gpio_level_high);
			}
			else
				gpio_set(GPIO_WLS, gpio_level_low); 
		  
			InitMsgInfo(&stMsg); 
		    printf("%s:%d: NETCMD_EMTR, mid = 0x%02X; flag = 0x%02X \n", __FILE__, __LINE__,  
				udpPacket.mid,g_setdevState.uEmtr);
 			break;
		case NETCMD_RESET:  // 报文长度：4+4+14=22 			
			g_setdevState.uReset = udpPacket.msg.reset;
		 	if(g_setdevState.uReset)
			{// ALS=1
				gpio_set(GPIO_RESET, gpio_level_high);
				gpio_set(GPIO_HLS, gpio_level_low);
				gpio_set(GPIO_VLS, gpio_level_low);
				gpio_set(GPIO_WLS, gpio_level_low);
				g_setdevState.uEmtr = 0x00;
			}
			
			gpio_set(GPIO_RESET, gpio_level_low); 

		    printf("%s:%d: NETCMD_RESET, mid = 0x%02X; reset=0x%02X; \n", __FILE__, __LINE__, 
				 udpPacket.mid,udpPacket.msg.reset);
			break; 
		default:
			break;
	}
  
 
} 
 
 
//////////////////////////////////////////////////////////////////////////