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
	COMM_VS = 0,  // 垂直发送          ttyS1
	COMM_HS,      // 水平发送          ttyS6
	COMM_AS,      // 全机发送          ttyS8 
	COMM_RESET,      // 全机发送         
	COMM_RECV,      // 串口收            
	COMM_NUM,    // 空
};
enum UDP_USER_TYPE
{// UDP网络用户类型
	UDP_USER_NULL = 0,
	UDP_USER_DH,  // 导航外设 
	UDP_USER_ST,  // 时统外设
	UDP_USER_WS3, 
	UDP_USER_WS4, 
	UDP_USER_ZB1,  // 组播1 101
	UDP_USER_ZB2,  // 组播2 107 
}; 


enum  UDPNET_MSG_STATE
{// 网络报文分解状态
	UDPMSG_STATE_IDLE = 0, // 无
	UDPMSG_STATE_HEAD,   // 有 报文头
	UDPMSG_STATE_MSGID,  // 有 MSGID 1字节
	UDPMSG_STATE_DATA,   // 数据
	UDPMSG_STATE_CHECK,  // 校验 异或和 1字节
	UDPMSG_STATE_PROC    // 已处理 
};
  

typedef struct
{	
	/*串口的收发缓冲区*/
	BYTE	rx_buf[UART_BUFF_SIZE];
	BYTE	tx_buf[UART_BUFF_SIZE];
	
	s16	rx_front;
	s16	rx_rear;
	
	s16	tx_front;
	s16	tx_rear;				
}UARTType;
 




enum    emEquipState
{
	ES_NORMAL = 0,  // 正常
	ES_FAULT, 
};
 
typedef  struct ComPara
{
	BOOL   IsOpen; // TRUE:已经打开
	int    baud;//波特率
	int    ComPort;//端口号
	int    data;// 数据位
	int    stop;   // 停止位
	char   Parity;  // 校验方式 ：Even、Odd、Non
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
{//网络参数
	BOOL  bIsOpen; // TRUE:已经打开
	BOOL  bAuto; //自动打开
	UINT   uNetRecvPort;//端口号
	UINT   uNetSendPort;//端口号
    char   chIP[16]; // 对方IP
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
// 获取一个完整的报文，返回值为报文类型，返回值=0表示报文不完整。

BOOL  IsMsgVailid(MSG_RECV_INFO  *stMsg); // 报文有效性检验
BYTE  EncodeMsgCheck(MSG_RECV_INFO  *pstMsg);    // 计算校验和 （异或和）



void  InitUDPMsgQueue(UDPMSG_INFO_LIST * pQ);   
void  InitUDPMsgInfo(UDPMSG_RECV_INFO * pMsg);
BYTE  FetchOneUDPMsg(UARTType &stBuff, UDPMSG_RECV_INFO& stNewMsg);
int   RecvOneUDPFrame(BYTE chData, UDPMSG_RECV_INFO *pMsg);
BOOL  IsUDPMsgVailid(UDPMSG_RECV_INFO  *pstMsg); // 报文有效性检验
BYTE  EncodeUDPMsgCheck(UDPMSG_RECV_INFO  stMsg);    // 计算校验和 （异或和）


int   PrintHex(MSG_RECV_INFO* pMsg);
int   HexToString(BYTE* pbuff, UINT ulen);


#endif
