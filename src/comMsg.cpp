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
		case COMMSG_VEMTR:  //  ״̬ѯ��  
			uID = EncodeComMsg_VEMTR(pMsg, pcomMsg);
			break;
		case COMMSG_HEMTR:  //  ״̬ѯ��  
			uID = EncodeComMsg_HEMTR(pMsg, pcomMsg);
			break;
		case COMMSG_AEMTR:  //  ״̬ѯ��  
			uID = EncodeComMsg_AEMTR(pMsg, pcomMsg);
			break;
		case COMMSG_RESET:  //  ��������У��֡
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
	case COMCMD_MLHC:  //  ����ش�֡
		break;
	case COMCMD_YXTX:  //  ����ͨ��֡
		break;
	case COMCMD_ZTHD:  //  ״̬ѯ�ʻش� 
 		break;
	case COMCMD_RQJG:  //  �������ش�֡
		break;
	case COMCMD_ZJML:  //  �Լ�����֡
		break; 
	case COMCMD_HALT:  //  �ػ�����֡
		break;
	case COMCMD_KJZJ:  //  ����״̬�Լ�֡�������յ���ش�����У��֡��ͨ��˫��������������3��4���͹���״̬���ġ�
		break;
	case COMCMD_HEART: //  ����֡
		break;

	default:
		uID = COMMSG_NULL;
		break;
	}

	return uID;
}
