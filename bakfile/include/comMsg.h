// comMsg.h: interface for the comMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_UART_COM_MSG_H)
#define _UART_COM_MSG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#include "Includes.h"

#pragma pack(2)


enum  COMMSG_TYPE
{// 本机 发送给内设 报文 
						 //   报文类型           接收方
	COMMSG_NULL = 0x00,   //  空 
	COMMSG_VEMTR = 0x01,  //  垂直发射  
	COMMSG_HEMTR = 0x02,  //  水平发送
	COMMSG_AEMTR = 0x03,  //  全机发送
	COMMSG_RESET = 0x04,  //  复位    
};

enum  COMCMD_TYPE
{// 本机 接收到的 内设命令  
	COMCMD_NULL   = 0x00,   //  空

	COMCMD_MLHC   = 0x01,   //  命令回传帧
	COMCMD_YXTX   = 0x02,   //  允许通信帧
	COMCMD_ZTHD   = 0x03,   //  状态询问回答
	COMCMD_RQJG   = 0x05,   //  认清结果回答帧
	COMCMD_ZJML   = 0x06,   //  自检命令帧
	COMCMD_HALT   = 0x08,   //  关机命令帧
	COMCMD_KJZJ   = 0x0A,   //  开机状态自检帧，本机收到后回传数据校验帧，通过双冗余网口向外设3、4发送工作状态报文。
	COMCMD_HEART  = 0x0B,   //  心跳帧
	
	COMCMD_QUIT  = 0xB1,   //  退出软件（关闭系统）
	COMCMD_OFF   = 0xB2,   //  关机（关闭系统）
	COMCMD_DEBUG  = 0xB3,   //  打开测试（输出调试信息）
}; 

typedef struct
{
	BYTE  head; 
	BYTE  mid;
	BYTE  len; 

}COMHEAD;
typedef struct
{// 18
	u8      count;//动点个数  (1--10)
	u8      sendaddr; //发方地址  0=外设3,1=外设4
	UINT    pihao;
	UINT    orient; 
	UINT    distan;
	UINT    info;

	UINT    No;
	UINT    state;
	UINT32    time;
}COM_SBJG;

typedef struct
{
	u8      count;//动点个数  (1--10)
	u8      sendaddr; //发方地址  0=外设3,1=外设4
	UINT    pihao;
	UINT    orient; 
	UINT    distan;
	UINT    mode;
}COM_SBSQ;


UINT  EncodeComMsg_ZJJG(void* pmsg, MSG_RECV_INFO  *pstMsg);
UINT  EncodeComMsg_SJJY(void* pmsg, MSG_RECV_INFO  *pcomMsg);
UINT  EncodeComMsg(UINT uID, void*  pMsg, MSG_RECV_INFO  *pcomMsg); // 本机发送到串口

UINT  DecodeComCmd(MSG_RECV_INFO stMsg);

#endif // !defined(AFX_COMMSG_H__DAC089CE_F9B3_4D36_8CC9_5831AFAD823A__INCLUDED_)
