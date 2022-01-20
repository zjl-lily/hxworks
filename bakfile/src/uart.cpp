//������ص�ͷ�ļ�  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
 
#include "../include/uart.h"
#include "../include/comMsg.h"


 
void InitUART(UARTType *pUART)
{
	int i;
	
	pUART->rx_front = 0;
	pUART->rx_rear = 0;
	
	pUART->tx_front = 0;
	pUART->tx_rear = 0;
	
	for(i=0;i<UART_BUFF_SIZE;i++)
	{
		pUART->rx_buf[i] = 0;
		pUART->tx_buf[i] = 0;
	}	
}

int	GetTxBufferLength(UARTType * pUART)
{
	int temp;

	temp = pUART->tx_rear - pUART->tx_front;

	return(temp >= 0 ? temp : temp + UART_BUFF_SIZE); 
}

int	GetRxBufferLength(UARTType * pUART)
{
	int temp;

	temp = pUART->rx_rear - pUART->rx_front;

	return(temp >= 0 ? temp : temp + UART_BUFF_SIZE); 
}

void EnRxBuffer(UARTType * pUART, int info)
{
	pUART->rx_buf[pUART->rx_rear] = info;
	pUART->rx_rear = (pUART->rx_rear == UART_BUFF_SIZE - 1) ? 0 : pUART->rx_rear + 1;
}

int DeRxBuffer(UARTType * pUART)
{
	int temp;

	temp = pUART->rx_buf[pUART->rx_front];
	pUART->rx_front = (pUART->rx_front == UART_BUFF_SIZE - 1) ? 0 : pUART->rx_front + 1;

	return(temp);
}

void EnTxBuffer(UARTType * pUART, int info)
{
	pUART->tx_buf[pUART->tx_rear] = info;
	pUART->tx_rear = (pUART->tx_rear == UART_BUFF_SIZE - 1) ? 0 : pUART->tx_rear + 1;
}

int DeTxBuffer(UARTType * pUART)
{
	int temp;
	temp = pUART->tx_buf[pUART->tx_front];
	pUART->tx_front = (pUART->tx_front == UART_BUFF_SIZE - 1) ? 0 : pUART->tx_front + 1;

	return temp;
}  
void InitMsgInfo(MSG_RECV_INFO * pMsg)
{	
 	pMsg->state = 0;
	pMsg->msgID = 0;
	pMsg->cnt = 0;
	pMsg->userID = 0;
	pMsg->len = 0;
	pMsg->ackNum = 0;
	pMsg->ackResult = 0;
	memset(pMsg->buff, '\0', MAX_MSG_LEN);
}
void InitMsgQueue(MSG_INFO_LIST * pQ)
{
	pQ->front = 0;
	pQ->rear = 0;
	int i = 0;
	MSG_RECV_INFO*  pMsg = NULL;
	for (i = 0; i < MAX_MSG_NUM; i++)
	{
		pMsg = &pQ->message[i];
		InitMsgInfo(pMsg);        
	}
}

void  InitUDPMsgInfo(UDPMSG_RECV_INFO * pMsg)
{
	pMsg->state = 0;
	pMsg->msgID = 0;
	pMsg->cnt = 0;
	pMsg->userID = 0;
	pMsg->len = 0;
	pMsg->ackNum = 0;
	pMsg->ackResult = 0;
	memset(pMsg->buff, '\0', MAX_MSG_LEN);
}
 
void  InitUDPMsgQueue(UDPMSG_INFO_LIST * pQ)
{
	pQ->front = 0;
	pQ->rear = 0;
	int i = 0;
	UDPMSG_RECV_INFO*  pMsg = NULL;
	for (i = 0; i < MAX_MSG_NUM; i++)
	{
		pMsg = &pQ->message[i];
		InitUDPMsgInfo(pMsg);        
	}
}

  

/*******************************************************************/
/*����û�����		                                               */
/*******************************************************************/
void GetCmd(UARTType *pUart)
{
	int rx_num; 

	rx_num = GetRxBufferLength(pUart);
 
}

bool  FetchOneUDPMsg(UARTType* pstBuff, UDPMSG_RECV_INFO* pstNewMsg)
{
	u8  byInfo = '\0';  
	u8  byLast = 0x00;
	int i = 0;
	int  len = GetRxBufferLength(pstBuff);
	for(i = 0;i < len; i++)
	{
		RecvOneUDPFrame(pstBuff->rx_buf[i], pstNewMsg);
	}

	return true;
}
int RecvOneUDPFrame(BYTE chData, UDPMSG_RECV_INFO *pMsg)
{
	int     nmsg = 0;
	int     rx_num;
	BYTE    info; 
	BYTE	check = 0x00; 
 	switch(pMsg->state)
	{
	case CMSG_STATE_IDLE:// ��
		if(chData != UDP_MSG_HEAD) 
			break;
    	pMsg->buff[0] = chData;
		pMsg->cnt = 1;
		pMsg->len = 0;
		pMsg->msgID = 0;
		pMsg->state = CMSG_STATE_HEAD;
		break;
	case CMSG_STATE_HEAD:// �� ֡ͷ  1�ֽ�
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		pMsg->msgID = chData;
		pMsg->state = CMSG_STATE_MSGID;
		break;
	case CMSG_STATE_MSGID:// �� MSGID 1�ֽ�
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		pMsg->len = chData; 
// 		printf("RecvOneComFrame: msglen = %d \r\n", pMsg->len);
		if (pMsg->len >= MAX_MSG_LEN)
		{
			InitUDPMsgInfo(pMsg);
			break;
		}
		else
			pMsg->state = CMSG_STATE_DATA;
		break;
	case CMSG_STATE_DATA:// ����
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;


		if(pMsg->cnt >= pMsg->len)
		{
 			if (IsUDPMsgVailid(pMsg))
			{// У��ɹ�
				pMsg->state = CMSG_STATE_CHECK; 
				nmsg++;
			}
			else
			{
				InitUDPMsgInfo(pMsg);
			}
		}
// 		printf("RecvOneComFrame: msglen = %d; recved = %d \r\n", pMsg->len,pMsg->cnt);

		break;
	default:
		break;
	}
 
	return nmsg;
}

BOOL  IsUDPMsgVailid(UDPMSG_RECV_INFO  *pstMsg) // ������Ч�Լ���
//��Ч���ģ�1û����ʼ��ʶ��2�����ֽ�����������ַ������������涨�Ĳ���
// 3.У��Ͳ���
{
 	if (pstMsg->cnt < 1)	return FALSE; //���ĳ��ȱ��� ���ڵ���5
    
	return TRUE;
 
	int i, nMsgLen;
	nMsgLen = pstMsg->cnt - 1;
// У��ͼ���
	BYTE  chChk1 = 0x00;
	BYTE* pBuff = pstMsg->buff;
 	for (i = 0; i < nMsgLen; i++)
	{
		chChk1 ^= pBuff[i];
	} 
	if (chChk1 == pBuff[nMsgLen])
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}	
}
BYTE  EncodeUDPMsgCheck(UDPMSG_RECV_INFO  stMsg)    // ����У��� �����ͣ�
{// strMsg = $...*
	if (stMsg.cnt < 1)
	{
		return 0x00;
	}
	BYTE  chTemp = 0x00;
	for (int i = 1; i < stMsg.cnt; i++)
	{
		chTemp ^= stMsg.buff[i];
	}
 	return chTemp;
}

BOOL IsMsgVailid(MSG_RECV_INFO*  pstMsg)//������Ч�Լ���
//��Ч���ģ�1û����ʼ��ʶ��2�����ֽ�����������ַ������������涨�Ĳ���
// 3.У��Ͳ���
{
 	if (pstMsg->cnt < 1)	return FALSE; //���ĳ��ȱ��� ���ڵ���5
    return TRUE;
	int i, nMsgLen;
	nMsgLen = pstMsg->cnt - 1;
// У��ͼ���
	BYTE  chChk1 = 0x00;
	BYTE* pBuff = pstMsg->buff;
 	for (i = 0; i < nMsgLen; i++)
	{
		chChk1 ^= pBuff[i];
	} 
	if (chChk1 == pBuff[nMsgLen])
	{
		return TRUE;
	}
	else 
	{
		return FALSE;
	}	
}
 
u8  EncodeMsgCheck(MSG_RECV_INFO  *pstMsg)
{//  cnt  �����ܳ���-1 
	if (!pstMsg || pstMsg->cnt < 1)
	{
		return 0x00;
	}
	BYTE  chTemp = 0x00;
	for (int i = 0; i < pstMsg->cnt; i++)
	{
		chTemp ^= pstMsg->buff[i];
	}
	pstMsg->buff[pstMsg->cnt] = chTemp;
	pstMsg->len += 1;
	pstMsg->cnt += 1;
 	return chTemp;
}
//////////////////////////////////////////////////////////////////////////
int HexToString(BYTE* pbuff, UINT ulen)
{
	if (!g_userDebug)
	{
		return 0;
	}
	if (!pbuff || ulen < 1)
	{
		return 0;
	}
	int n = 0;
	char buffer[300] = {'\0'};
	int  index = 0;
 	for (n = 0; n < ulen && n < 100; n++)
	{
		index += sprintf(buffer+index, "%02X ", pbuff[n]);
	}
    printf("%s:%d, %s\n", __FILE__, __LINE__, buffer);	
	return 0;
}

int PrintHex(MSG_RECV_INFO* pMsg)
{
	if (!g_userDebug)
	{
		return 0;
	}
	if (!pMsg || pMsg->len < 1)
	{
		return 0;
	}
	int n = 0;
	char buffer[100] = {'\0'};
	int  index = 0;
 	for (n = 0; n < pMsg->len && n < 100; n++)
	{
		index += sprintf(buffer+index, "%02X ", pMsg->buff[n]);
	}
    printf("msg:%s \n", buffer);
	
	return 0;
}
