//SerialPort.h  这是串口类的头文件，定义了一个串口了
#ifndef _UART_SERIALPORT_H
#define _UART_SERIALPORT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/uart.h"

/*linux下串口需要使用到的头文件*/

#include<termios.h>       

enum  COMSEND_ERR
{
	COMERR_NULL = 0,  //  
	COMERR_OUTTIMES,  // 超时 》18
	COMERR_TXOUT,         // 通信申请次数超时

};

class SerialPort
{

public:

	UARTType  m_stUart;
	MSG_INFO_LIST  m_stSMsgList;//
    MSG_INFO_LIST  m_stRMsgList;//
    MSG_RECV_INFO  m_stRecvMsg; // 正在接收的
	BYTE  m_state; // 串口状态 0x00=正确，0x80=故障
	MSG_INFO_QUENE m_recvQuene;// 报文接收队列 
	MSG_INFO_QUENE m_sendQuene;// 报文发送队列
	COMCMD_INFO_TYPE  m_stCmd; // 通信申请

    bool  m_bRecv;
    bool  m_bSend;
	u8    m_userid;
    char  m_DevName[16];             // 串口设备名称   

    SerialPort();
    SerialPort(int WhichCom);
    ~SerialPort();

	void     setcompara(ComPara stpara);
	ComPara  getcompara();
  
	
	bool setPortPara(ComPara stPara); // 初始化串口
    bool InitPort(int BaudRate,int DataBits,int StopBits,char ParityBit); // 初始化串口
  
	bool OpenPort(ComPara stPara); // 打开串口
	bool ClosePort();// 关闭串口

    int WritePort(void *Buff, u16 Len);  // 发送  向串口写入数据
    int ReadPort(void *Buff,  u16 Len);  // 接收  从串口中读取数据

 
	int  FetchOneMsg(BYTE* pinfo, u16 len); // 分解一个完整报文


	MSG_RECV_INFO*   GetOneRecvMsg(); //  从报文列表中提取一个指定状态的报文
	int	  RecvOneComFrame(BYTE chData, MSG_RECV_INFO *pMsg);
	

	int  SendOneMsg(MSG_RECV_INFO *pmsg); // 发送一个报文
	int  SendMsg();  // 发送全部报文
 
	bool  AddSendInfo(char* pinfo, int len); // 将需要发送的报文放到  发送报文列表
	bool  AddSendMsg(MSG_RECV_INFO msg); // 将需要发送的报文放到  发送报文列表

	int   RecvMsg(); // 接收数据并分解为独立报文
	
	int   getFD();

	BYTE  getState();
	void  SetState(BYTE ustate);

	UINT  getCmd_MLHC();
	void  setCmd_MLHC(UINT uMLHC);

// 	//////////////////////////////////////////////////////////////////////////
// 	// 本机 发送给 串口消息队列
// 	MSG_RECV_INFO*  FirstSendMsg(); // 
// 	void  pushSendMsg(MSG_RECV_INFO *pMsg);// 添加一条消息
// 	MSG_RECV_INFO*  popSendMsg();  // 取出一条消息
// 	int   sendMsgCount(); //  发送消息队列中数据个数
	void ProcessCom_SendCmd(u8 uid);
	void ProcessComSendErr(COMSEND_ERR  uerr);

private:
    struct termios m_Setting;           // 串口配置的结构体
	ComPara  m_stComPara;
    int m_fd;                             // 打开串口设备后返回的文件描述符
 
 
};

 

#endif 
