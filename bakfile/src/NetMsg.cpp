// NetMsg.cpp: implementation of the NetMsg class.
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>  

#include "../include/NetMsg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
DEVICE_STATE     g_setdevState;  
RONGYU_TEST      g_stRongYu;

#define   NETMSG_MAX_TIME   863999999  // 网络报文中时间的最大值 23:59:59'999.9
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
  
  
UINT  EncodeNetMsg_RQJG(void* pPara, void* pMsg)
{  // //  识别应答（认清结果）报文    组播1（224.21.200.101）
  

	return NETMSG_RQJG;
} 
UINT  EncodeOneNetMsg(UINT uType,  void* pPara,  void* pMsg)
{
 	BYTE  uMsgLen = 0;
	UINT  uIndex = 0;  
    UINT  uID = NETMSG_NULL; 
	switch(uType)
	{
	case NETMSG_GZZT:   //  工作状态报文    组播2（224.21.200.107）
 		break;
	case NETMSG_RQJG:   //  识别应答（认清结果）报文    组播1（224.21.200.101）
 		break;
	case NETMSG_HEART:  //  工作状态报文    组播2（224.21.200.107）
 		break;
	case NETMSG_HALT:   //  工作状态报文    组播2（224.21.200.107）
 		break;
	default:
		break;
	}    
	return uID;

}
int  EncodeNet_IPAddr(BYTE*  pBuff, const char*  pIP) // ID地址
{
	UINT32  uIP = inet_addr(pIP);
	return  u32ToBuff(pBuff, uIP);


	return 4;
}
int EncodeNet_Time4(BYTE*  pInfo, TIMESTRUCT stTime)
{ 
	UINT32 uT = stTime.ToLong();
	BYTE* pd = (BYTE*)&uT;
	pInfo[0] = pd[3];
	pInfo[1] = pd[2];
	pInfo[2] = pd[1];
	pInfo[3] = pd[0];
	return 4;
}
int  EncodeNet_Pihao(BYTE*  pInfo, UINT uPihao)
{
	char strT[6];
	sprintf(strT, "%04d", uPihao);
	pBCD_BYTE  pBCD = (pBCD_BYTE)&pInfo[0];
	pBCD->byBCD1 = strT[2];
	pBCD->byBCD0 = strT[3];

	pBCD = (pBCD_BYTE)&pInfo[1];
	pBCD->byBCD1 = strT[0];
	pBCD->byBCD0 = strT[1];

	return 2;
} 
int  EncodeNet_ShipID(BYTE*  pInfo, UINT32 uShipID)
{
	char strT[6];
	sprintf(strT, "%05l", uShipID);
	pBCD_BYTE  pBCD = (pBCD_BYTE)&pInfo[0];
	pBCD->byBCD1 = strT[3];
	pBCD->byBCD0 = strT[4];
	
	pBCD = (pBCD_BYTE)&pInfo[1];
	pBCD->byBCD1 = strT[1];
	pBCD->byBCD0 = strT[2];

	pBCD = (pBCD_BYTE)&pInfo[2]; 
	pBCD->byBCD0 = strT[0];

	return 4;
}

