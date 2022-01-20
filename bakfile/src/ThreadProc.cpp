// ThreadProc.cpp: implementation of the ThreadProc class.
//
//////////////////////////////////////////////////////////////////////

#include "../include/ThreadProc.h"
#include "../include/sysProc.h"
#include <time.h>
#include <sys/time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TH_INFO_TYPE  g_ptrThread[TH_NUM];

void *UDPRecv_Func(void *arg);
void *WriteFunction(void *arg); 
void *UDPRecv_Func(void *arg);
void *ProcessUDPMsg(void *arg);

 //////////////////////////////////////////////////////////////////////
  // 线程体
 void *RongYu_Func(void *arg)
 {//  冗余测试
  	struct timezone tz_cur; 
	struct tm *ptm;
	struct timeval  tv_cur;
 	time_t  tp;


    int   msglen = 0;
    int   len = 0;
	char  pdata[100] = {'\0'};

	long  ltimes = 0;
	long  lcounts = g_stRongYu.counts;
	u16   delt = g_stRongYu.delt * 1000;
	sprintf(pdata, " count = %6ld, delt = %dms ", lcounts, delt);
	DebugPrintf(__FILE__, __FUNCTION__, __LINE__, pdata);


    float  fradio = 0;
	UDPSocket*  pUDP = NULL;
	g_ptrThread[TH_RONGYU].flag = 1;
	while(ltimes < lcounts && g_ptrThread[TH_RONGYU].flag == true && ptrSP.m_pUDPSock)
    {  
		pUDP = ptrSP.m_pUDPSock;

		time(&tp);
    	ptm = localtime(&tp);
    	gettimeofday(&tv_cur, &tz_cur); 
		ltimes++; 
		len = sprintf(pdata,"[%02d:%02d.%ld]: No.%ld  RongYu_Func",  
			ptm->tm_min,ptm->tm_sec, tv_cur.tv_usec/1000, ltimes);
		if (len > 0)
		{
			msglen = pUDP->UDPSendData(pdata, len, UDP_USER_WS4);
			msglen = pUDP->UDPSendData(pdata, len, UDP_USER_WS3);
			if (msglen > 0)
			{
				myprintf("%s, msglen=%d \n", pdata, msglen);
			}
			else
				myprintf("UDPSendData Error \n");
		}
		usleep(delt); 
    } 
	g_stRongYu.flag = 0;
	g_ptrThread[TH_RONGYU].flag = false;
 }
// 线程体
 void *Timer_Func(void *arg)
 {
	DebugPrintf(__FILE__, __FUNCTION__, __LINE__);
	int   times = 1;
	int   i = 0;
 	UDPSocket*  pUDP = ptrSP.m_pUDPSock;

	struct timeval  tv_old;
	struct timezone tz_old;
	struct timeval  tv_cur;
	struct timezone tz_cur;
	long    udelt = 0;
	g_ptrThread[TH_TIMER].flag = 1;
    while(g_ptrThread[TH_TIMER].flag == true)
    { 
		times++;
		if (pUDP && times > 2)
		{// 定时检测网口状态   1s
			times = 1;
			pUDP->check_netstate();
		}
		usleep(500000); // 500ms
    } 
	g_ptrThread[TH_TIMER].flag = 0;

 }
 void *UDPSend_Func(void *arg)
{
	UDPSocket*  pUDP = ptrSP.m_pUDPSock;
	if (!pUDP)
	{
		printf("UDPSend_Func: error \n");
		return NULL;
	} 
	g_ptrThread[TH_UDPSEND].flag = 1;
    while(g_ptrThread[TH_UDPSEND].flag == true && pUDP)
	{
		 pUDP->SendMsg();		 
	}
	g_ptrThread[TH_UDPSEND].flag = 0;

}
void *UDPMsg_Func(void *arg)
{// 处理收到的网络报文
	UDPSocket* pUDP = ptrSP.m_pUDPSock; 
	g_ptrThread[TH_PROCCMD].flag = 1;
	while(pUDP && g_ptrThread[TH_PROCCMD].flag == true )
	{
		ptrSP.ProcessUDPCmd();
	}
	g_ptrThread[TH_PROCCMD].flag = 0;

}
void *UDPRecv_Func(void *arg)
{
	UDPSocket*  pUDP = ptrSP.m_pUDPSock;
	g_ptrThread[TH_UDPRECV].flag = 1;
    if(pUDP && g_ptrThread[TH_UDPRECV].flag == true)
	{
		pUDP->UDPRecv(); 
	}
    g_ptrThread[TH_UDPRECV].flag = 0;

} 
//  线程体，不断地写入数据
void *WriteFunction(void *arg)
{
    int  i = 0;    
    SerialPort *pSP = NULL;
	g_ptrThread[TH_COMSEND].flag = 1; 
    while(g_ptrThread[TH_COMSEND].flag == 1)
    {
		for(i = COMM_VS; i < COMM_NUM; i++)
	   {
		    pSP  = ptrSP.m_pSPlist[i];
		    if(pSP && pSP->m_bSend)
			{
				pSP->SendMsg();
			}
	   } 
    }
}

//  线程体，不断地读取数据
void *ReadFunction(void *arg)
{ 
    SerialPort *pSP = ptrSP.m_pSPlist[COMM_RECV];
	MSG_RECV_INFO* pMsg = NULL;
 	if (!pSP || !pSP->m_bRecv)
	{
		return NULL;
	}
	g_ptrThread[TH_COMRECV].flag = true;
    while(g_ptrThread[TH_COMRECV].flag == true)
    { 
 		if (pSP && pSP->m_bRecv)
		{
			pSP->RecvMsg();
			if (pSP->m_recvQuene.size() > 0)
			{
				pMsg = pSP->GetOneRecvMsg();  
				if(pMsg && (pMsg->state == CMSG_STATE_CHECK || pMsg->state == CMSG_STATE_CHKERR))
				{
  					ptrSP.ProcessOneComCmd(pMsg); 
					InitMsgInfo(pMsg); 
				}
			}
		} 
    }
	g_ptrThread[TH_COMRECV].flag = false;
    return NULL;
}
//////////////////////////////////////////////////////////////////////////
ThreadProc::ThreadProc()
{
	for (int i = 0; i < TH_NUM; i++)
	{
		g_ptrThread[i].tid = -1;
		g_ptrThread[i].name = TH_NUM;
		g_ptrThread[i].flag = false;
		g_ptrThread[i].delt = 1;
		g_ptrThread[i].counts = 1;
		g_ptrThread[i].type = THTYPE_CIRCLE; 
	}  
	g_stRongYu.counts = 1;
	g_stRongYu.delt = 20;
	g_stRongYu.flag = 0;

}
ThreadProc::~ThreadProc()
{
	stopThread();
}
//////////////////////////////////////////////////////////////////////
void   ThreadProc::setTimerInterval(UINT milsecs)
{
	g_ptrThread[TH_TIMER].delt = milsecs;
}
void   ThreadProc::start(THREAD_NAME uT)
{
	int ret = 0;
 	switch(uT)
	{ 
	case TH_COMRECV:
		ret = pthread_create(&g_ptrThread[uT].tid, NULL, ReadFunction, NULL); 
		break;	 
	case TH_COMSEND:
		ret = pthread_create(&g_ptrThread[uT].tid, NULL, WriteFunction, NULL); 
		break;
	case TH_UDPRECV:
		ret = pthread_create(&g_ptrThread[uT].tid, NULL, UDPRecv_Func, NULL); 
		break;
	case TH_UDPSEND:
		ret = pthread_create(&g_ptrThread[uT].tid, NULL, UDPSend_Func, NULL); 
		break;
	case TH_PROCCMD:
		ret = pthread_create(&g_ptrThread[uT].tid, NULL, UDPMsg_Func, NULL); 
		break;
	case TH_TIMER:
		ret = pthread_create(&g_ptrThread[uT].tid, NULL, Timer_Func, NULL); 
		break;
	case TH_RONGYU:
		ret = pthread_create(&g_ptrThread[uT].tid, NULL, RongYu_Func, NULL); 
		break;
	default:
		return;
		break;
	}
 
	if (ret)
	{
		g_ptrThread[uT].name = uT;
		char pdata[100]; 
		sprintf(pdata, "  created thread : %d   Error \n", uT);
		DebugPrintf(__FILE__, __FUNCTION__, __LINE__,  pdata);
 	    g_ptrThread[uT].flag = false;	
	}
	else
	{
		char pdata[100]; 
		sprintf(pdata, "  created thread : %d   sucessed \n", uT);
		DebugPrintf(__FILE__, __FUNCTION__, __LINE__,  pdata); 
	    g_ptrThread[uT].flag = true;
	}
}
void   ThreadProc::stop(THREAD_NAME uT)
{
	if (g_ptrThread[uT].flag)
	{
		g_ptrThread[uT].flag = 0;
		if (TH_UDPRECV == uT || TH_UDPSEND == uT)
		{
 			ptrSP.m_pUDPSock->shutdownUDP(); 
		}
		pthread_join(g_ptrThread[uT].tid, NULL);
		printf("stop  thread %d \n", uT);
	}
	else
		printf("thread %d  had stopped \n", uT);
 	
} 
bool   ThreadProc::isrun(THREAD_NAME uT)
{
	return   g_ptrThread[uT].flag;
}

void  ThreadProc::startThread()
{ 
/*	start(TH_COMRECV);
	start(TH_COMSEND);
 	start(TH_TIMER);

	start(TH_UDPSEND);*/
	start(TH_UDPRECV);
	start(TH_PROCCMD);
	
}
void  ThreadProc::stopThread()
{
	stop(TH_PROCCMD); 
	stop(TH_UDPRECV);
/*	stop(TH_UDPSEND);
	
 	stop(TH_TIMER); 
	stop(TH_COMSEND);	
	stop(TH_COMRECV);
	*/
} 
//////////////////////////////////////////////////////////////////////////
// 冗余测试
void  ThreadProc::startRongYu()
{
	if (g_stRongYu.flag == 1)
	{
  		start(TH_RONGYU);
	}
}
void  ThreadProc::stopRongYu()
{
	if (g_stRongYu.flag == 1)
	{
		g_stRongYu.flag = 0;
 		stop(TH_RONGYU);
	}
}


