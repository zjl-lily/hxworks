//SerialPort.h  ���Ǵ������ͷ�ļ���������һ��������
#ifndef _UART_SERIALPORT_H
#define _UART_SERIALPORT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/uart.h"

/*linux�´�����Ҫʹ�õ���ͷ�ļ�*/

#include<termios.h>       

enum  COMSEND_ERR
{
	COMERR_NULL = 0,  //  
	COMERR_OUTTIMES,  // ��ʱ ��18
	COMERR_TXOUT,         // ͨ�����������ʱ

};

class SerialPort
{

public:

	UARTType  m_stUart;
	MSG_INFO_LIST  m_stSMsgList;//
    MSG_INFO_LIST  m_stRMsgList;//
    MSG_RECV_INFO  m_stRecvMsg; // ���ڽ��յ�
	BYTE  m_state; // ����״̬ 0x00=��ȷ��0x80=����
	MSG_INFO_QUENE m_recvQuene;// ���Ľ��ն��� 
	MSG_INFO_QUENE m_sendQuene;// ���ķ��Ͷ���
	COMCMD_INFO_TYPE  m_stCmd; // ͨ������

    bool  m_bRecv;
    bool  m_bSend;
	u8    m_userid;
    char  m_DevName[16];             // �����豸����   

    SerialPort();
    SerialPort(int WhichCom);
    ~SerialPort();

	void     setcompara(ComPara stpara);
	ComPara  getcompara();
  
	
	bool setPortPara(ComPara stPara); // ��ʼ������
    bool InitPort(int BaudRate,int DataBits,int StopBits,char ParityBit); // ��ʼ������
  
	bool OpenPort(ComPara stPara); // �򿪴���
	bool ClosePort();// �رմ���

    int WritePort(void *Buff, u16 Len);  // ����  �򴮿�д������
    int ReadPort(void *Buff,  u16 Len);  // ����  �Ӵ����ж�ȡ����

 
	int  FetchOneMsg(BYTE* pinfo, u16 len); // �ֽ�һ����������


	MSG_RECV_INFO*   GetOneRecvMsg(); //  �ӱ����б�����ȡһ��ָ��״̬�ı���
	int	  RecvOneComFrame(BYTE chData, MSG_RECV_INFO *pMsg);
	

	int  SendOneMsg(MSG_RECV_INFO *pmsg); // ����һ������
	int  SendMsg();  // ����ȫ������
 
	bool  AddSendInfo(char* pinfo, int len); // ����Ҫ���͵ı��ķŵ�  ���ͱ����б�
	bool  AddSendMsg(MSG_RECV_INFO msg); // ����Ҫ���͵ı��ķŵ�  ���ͱ����б�

	int   RecvMsg(); // �������ݲ��ֽ�Ϊ��������
	
	int   getFD();

	BYTE  getState();
	void  SetState(BYTE ustate);

	UINT  getCmd_MLHC();
	void  setCmd_MLHC(UINT uMLHC);

// 	//////////////////////////////////////////////////////////////////////////
// 	// ���� ���͸� ������Ϣ����
// 	MSG_RECV_INFO*  FirstSendMsg(); // 
// 	void  pushSendMsg(MSG_RECV_INFO *pMsg);// ���һ����Ϣ
// 	MSG_RECV_INFO*  popSendMsg();  // ȡ��һ����Ϣ
// 	int   sendMsgCount(); //  ������Ϣ���������ݸ���
	void ProcessCom_SendCmd(u8 uid);
	void ProcessComSendErr(COMSEND_ERR  uerr);

private:
    struct termios m_Setting;           // �������õĽṹ��
	ComPara  m_stComPara;
    int m_fd;                             // �򿪴����豸�󷵻ص��ļ�������
 
 
};

 

#endif 
