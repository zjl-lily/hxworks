#ifndef _UART_SET_H
#define _UART_SET_H 

#pragma once

#include "Includes.h"


#define UART_BUFF_SIZE 	1024
enum {COM_SEND = 0, COM_RECV};
#define  COMM_MSG_HEAD  0x3F
#define  UDP_MSG_HEAD   0x7F


 
 
enum    COMM_USER_TYPE
{
	COMM_VS = 0,  // ��ֱ����          ttyS1
	COMM_HS,      // ˮƽ����          ttyS6
	COMM_AS,      // ȫ������          ttyS8 
	COMM_RESET,      // ȫ������         
	COMM_RECV,      // ������            
	COMM_NUM,    // ��
};
enum UDP_USER_TYPE
{// UDP�����û�����
	UDP_USER_NULL = 0,
	UDP_USER_DH,  // �������� 
	UDP_USER_ST,  // ʱͳ����
	UDP_USER_WS3, 
	UDP_USER_WS4, 
	UDP_USER_ZB1,  // �鲥1 101
	UDP_USER_ZB2,  // �鲥2 107 
}; 


enum  UDPNET_MSG_STATE
{// ���籨�ķֽ�״̬
	UDPMSG_STATE_IDLE = 0, // ��
	UDPMSG_STATE_HEAD,   // �� ����ͷ
	UDPMSG_STATE_MSGID,  // �� MSGID 1�ֽ�
	UDPMSG_STATE_DATA,   // ����
	UDPMSG_STATE_CHECK,  // У�� ���� 1�ֽ�
	UDPMSG_STATE_PROC    // �Ѵ��� 
};
  

typedef struct
{	
	/*���ڵ��շ�������*/
	BYTE	rx_buf[UART_BUFF_SIZE];
	BYTE	tx_buf[UART_BUFF_SIZE];
	
	s16	rx_front;
	s16	rx_rear;
	
	s16	tx_front;
	s16	tx_rear;				
}UARTType;
 




enum    emEquipState
{
	ES_NORMAL = 0,  // ����
	ES_FAULT, 
};
 
typedef  struct ComPara
{
	BOOL   IsOpen; // TRUE:�Ѿ���
	int    baud;//������
	int    ComPort;//�˿ں�
	int    data;// ����λ
	int    stop;   // ֹͣλ
	char   Parity;  // У�鷽ʽ ��Even��Odd��Non
 	ComPara()
	{
		IsOpen = FALSE;
		ComPort = 1;
		baud = 115200;
		data = 8;
		stop = 1;
		Parity = 'n';		 
	}
	const ComPara& operator=(ComPara  sttemp)
	{
		this->IsOpen = sttemp.IsOpen;
		this->ComPort = sttemp.ComPort;
		this->baud = sttemp.baud;
		this->data = sttemp.data; 
		this->stop = sttemp.stop;
		this->Parity = sttemp.Parity;
		return  *this;
	}
}COM_PARA;

typedef  struct NetPara
{//�������
	BOOL  bIsOpen; // TRUE:�Ѿ���
	BOOL  bAuto; //�Զ���
	UINT   uNetRecvPort;//�˿ں�
	UINT   uNetSendPort;//�˿ں�
    char   chIP[16]; // �Է�IP
 	NetPara()
	{
		bIsOpen = FALSE;
		bAuto = FALSE;
		uNetRecvPort =  1000;
		uNetSendPort =  1001;
	};
	void  Init()
	{  
		bAuto = FALSE;
		bIsOpen = FALSE;
		uNetRecvPort =  1000;
		uNetSendPort =  1001;
	};
	const NetPara& operator=(NetPara  sttemp)
	{
		this->bIsOpen = sttemp.bIsOpen;
		this->bAuto = sttemp.bAuto;
		this->uNetRecvPort = sttemp.uNetRecvPort;
		this->uNetSendPort = sttemp.uNetSendPort;
 		return  *this;
	}
}NET_PARA;
 





void    InitUART(UARTType *pUART);
int	    GetTxBufferLength(UARTType *pUART);
int	    GetRxBufferLength(UARTType *pUART);
void    EnRxBuffer(UARTType *pUART,int info);
int     DeRxBuffer(UARTType *pUART);		
void    EnTxBuffer(UARTType *pUART,int info);
int     DeTxBuffer(UARTType *pUART); 



void	InitMsgQueue(MSG_INFO_LIST * pQ);   
void	InitMsgInfo(MSG_RECV_INFO * pMsg);
// ��ȡһ�������ı��ģ�����ֵΪ�������ͣ�����ֵ=0��ʾ���Ĳ�������

BOOL  IsMsgVailid(MSG_RECV_INFO  *stMsg); // ������Ч�Լ���
BYTE  EncodeMsgCheck(MSG_RECV_INFO  *pstMsg);    // ����У��� �����ͣ�



void  InitUDPMsgQueue(UDPMSG_INFO_LIST * pQ);   
void  InitUDPMsgInfo(UDPMSG_RECV_INFO * pMsg);
BYTE  FetchOneUDPMsg(UARTType &stBuff, UDPMSG_RECV_INFO& stNewMsg);
int   RecvOneUDPFrame(BYTE chData, UDPMSG_RECV_INFO *pMsg);
BOOL  IsUDPMsgVailid(UDPMSG_RECV_INFO  *pstMsg); // ������Ч�Լ���
BYTE  EncodeUDPMsgCheck(UDPMSG_RECV_INFO  stMsg);    // ����У��� �����ͣ�


int   PrintHex(MSG_RECV_INFO* pMsg);
int   HexToString(BYTE* pbuff, UINT ulen);


#endif
