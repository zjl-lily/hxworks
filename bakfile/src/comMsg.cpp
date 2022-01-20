// comMsg.cpp: implementation of the comMsg class.
//
//////////////////////////////////////////////////////////////////////

#include "../include/comMsg.h"
#include "../include/NetMsg.h"
#include "../include/sysProc.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


UINT  EncodeComMsg_RESET(void*  pMsg, MSG_RECV_INFO  *pcomMsg)
{
	UINT  index = 0; 
	printf("%s:%s:%d: RESET\n", __FILE__, __FUNCTION__, __LINE__);
 /*
	BYTE *pDst = pcomMsg->buff;  
	pcomMsg->buff[index] = COMM_MSG_HEAD;   index++;
	pcomMsg->buff[index] = COMMSG_RESET  ;  index++;
	pcomMsg->buff[index] = 5;    index++;
 	pcomMsg->buff[index] = g_setdevState.uReset&0x01;    index++;  
	pcomMsg->cnt = index;
	pcomMsg->len = index;
	EncodeMsgCheck(pcomMsg); 
*/
	pcomMsg->buff[index] = g_setdevState.uReset&0x0001;    index++;
	pcomMsg->cnt = index;
	pcomMsg->len = index;
	HexToString(pcomMsg->buff, pcomMsg->cnt);
	pcomMsg->state = CMSG_STATE_CHECK; 	
	return COMMSG_RESET;
}
UINT  EncodeComMsg_VEMTR(void*  pMsg, MSG_RECV_INFO  *pcomMsg)
{
	UINT  index = 0; 
	printf("%s:%s:%d: VEMTR\n", __FILE__, __FUNCTION__, __LINE__);
 /*
	BYTE *pDst = pcomMsg->buff;  

	pcomMsg->buff[index] = COMM_MSG_HEAD;  index++;
	pcomMsg->buff[index] = COMMSG_VEMTR;    index++;
	pcomMsg->buff[index] = 5;             index++;
 	pcomMsg->buff[index] = g_setdevState.uEmtr&0x02;    index++;
   
	pcomMsg->cnt = index;
	pcomMsg->len = index;
	EncodeMsgCheck(pcomMsg); 

 
  */
	pcomMsg->buff[index] = ((g_setdevState.uEmtr&0x02)>>1);    index++;
	pcomMsg->cnt = index;
	pcomMsg->len = index;
	pcomMsg->state = CMSG_STATE_CHECK; 	
	HexToString(pcomMsg->buff, pcomMsg->cnt);
	return COMMSG_VEMTR;
}  

UINT  EncodeComMsg_HEMTR(void*  pMsg, MSG_RECV_INFO  *pcomMsg)
{ 
	UINT  index = 0; 
	printf("%s:%s:%d:HEMTR\n", __FILE__, __FUNCTION__, __LINE__);
/*
	BYTE *pDst = pcomMsg->buff;  
	pcomMsg->buff[index] = COMM_MSG_HEAD;  index++;
	pcomMsg->buff[index] = COMMSG_HEMTR;    index++;
	pcomMsg->buff[index] = 5;             index++;
 	pcomMsg->buff[index] = g_setdevState.uEmtr&0x01;    index++;
   
	pcomMsg->cnt = index;
	pcomMsg->len = index;
	EncodeMsgCheck(pcomMsg); 

*/
 	pcomMsg->buff[index] = g_setdevState.uEmtr&0x01;    index++;
	pcomMsg->cnt = index;
	pcomMsg->len = index; 
	HexToString(pcomMsg->buff, pcomMsg->cnt);
	pcomMsg->state = CMSG_STATE_CHECK; 	
	return COMMSG_HEMTR;
}  

UINT  EncodeComMsg_AEMTR(void*  pMsg, MSG_RECV_INFO  *pcomMsg)
{ 
	UINT  index = 0; 
	printf("%s:%s:%d: AEMTR\n", __FILE__, __FUNCTION__, __LINE__);
/*
	BYTE *pDst = pcomMsg->buff;  
	pcomMsg->buff[index] = COMM_MSG_HEAD;  index++;
	pcomMsg->buff[index] = COMMSG_AEMTR;    index++;
	pcomMsg->buff[index] = 5;             index++;
 	pcomMsg->buff[index] = g_setdevState.uEmtr&0x03;    index++;
   
	pcomMsg->cnt = index;
	pcomMsg->len = index;
	EncodeMsgCheck(pcomMsg);  
*/
 	pcomMsg->buff[index] = ((g_setdevState.uEmtr&0x04)>>2);    index++;
	pcomMsg->cnt = index;
	pcomMsg->len = index; 
	HexToString(pcomMsg->buff, pcomMsg->cnt); 	
	pcomMsg->state = CMSG_STATE_CHECK; 	
	return COMMSG_AEMTR;
}  


UINT  EncodeComMsg(UINT uID, void*  pMsg, MSG_RECV_INFO  *pcomMsg)
{	
	switch(uID)
	{
		case COMMSG_VEMTR:  //  状态询问  
			uID = EncodeComMsg_VEMTR(pMsg, pcomMsg);
			break;
		case COMMSG_HEMTR:  //  状态询问  
			uID = EncodeComMsg_HEMTR(pMsg, pcomMsg);
			break;
		case COMMSG_AEMTR:  //  状态询问  
			uID = EncodeComMsg_AEMTR(pMsg, pcomMsg);
			break;
		case COMMSG_RESET:  //  接收数据校验帧
			uID = EncodeComMsg_RESET(pMsg, pcomMsg);
			break; 
		default:
			uID = COMMSG_NULL;
			break;
	}

	return uID;
}
//////////////////////////////////////////////////////////////////////////
 
 
UINT  DecodeComCmd(MSG_RECV_INFO stMsg)
{
  	UINT  index = 0; 
	BYTE *pDst = stMsg.buff; 
	UINT  uID = stMsg.buff[1];
	switch(uID)
	{
	case COMCMD_MLHC:  //  命令回传帧
		break;
	case COMCMD_YXTX:  //  允许通信帧
		break;
	case COMCMD_ZTHD:  //  状态询问回答 
 		break;
	case COMCMD_RQJG:  //  认清结果回答帧
		break;
	case COMCMD_ZJML:  //  自检命令帧
		break; 
	case COMCMD_HALT:  //  关机命令帧
		break;
	case COMCMD_KJZJ:  //  开机状态自检帧，本机收到后回传数据校验帧，通过双冗余网口向外设3、4发送工作状态报文。
		break;
	case COMCMD_HEART: //  心跳帧
		break;

	default:
		uID = COMMSG_NULL;
		break;
	}

	return uID;
}
