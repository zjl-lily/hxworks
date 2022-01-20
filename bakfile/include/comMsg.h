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
{// ���� ���͸����� ���� 
						 //   ��������           ���շ�
	COMMSG_NULL = 0x00,   //  �� 
	COMMSG_VEMTR = 0x01,  //  ��ֱ����  
	COMMSG_HEMTR = 0x02,  //  ˮƽ����
	COMMSG_AEMTR = 0x03,  //  ȫ������
	COMMSG_RESET = 0x04,  //  ��λ    
};

enum  COMCMD_TYPE
{// ���� ���յ��� ��������  
	COMCMD_NULL   = 0x00,   //  ��

	COMCMD_MLHC   = 0x01,   //  ����ش�֡
	COMCMD_YXTX   = 0x02,   //  ����ͨ��֡
	COMCMD_ZTHD   = 0x03,   //  ״̬ѯ�ʻش�
	COMCMD_RQJG   = 0x05,   //  �������ش�֡
	COMCMD_ZJML   = 0x06,   //  �Լ�����֡
	COMCMD_HALT   = 0x08,   //  �ػ�����֡
	COMCMD_KJZJ   = 0x0A,   //  ����״̬�Լ�֡�������յ���ش�����У��֡��ͨ��˫��������������3��4���͹���״̬���ġ�
	COMCMD_HEART  = 0x0B,   //  ����֡
	
	COMCMD_QUIT  = 0xB1,   //  �˳�������ر�ϵͳ��
	COMCMD_OFF   = 0xB2,   //  �ػ����ر�ϵͳ��
	COMCMD_DEBUG  = 0xB3,   //  �򿪲��ԣ����������Ϣ��
}; 

typedef struct
{
	BYTE  head; 
	BYTE  mid;
	BYTE  len; 

}COMHEAD;
typedef struct
{// 18
	u8      count;//�������  (1--10)
	u8      sendaddr; //������ַ  0=����3,1=����4
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
	u8      count;//�������  (1--10)
	u8      sendaddr; //������ַ  0=����3,1=����4
	UINT    pihao;
	UINT    orient; 
	UINT    distan;
	UINT    mode;
}COM_SBSQ;


UINT  EncodeComMsg_ZJJG(void* pmsg, MSG_RECV_INFO  *pstMsg);
UINT  EncodeComMsg_SJJY(void* pmsg, MSG_RECV_INFO  *pcomMsg);
UINT  EncodeComMsg(UINT uID, void*  pMsg, MSG_RECV_INFO  *pcomMsg); // �������͵�����

UINT  DecodeComCmd(MSG_RECV_INFO stMsg);

#endif // !defined(AFX_COMMSG_H__DAC089CE_F9B3_4D36_8CC9_5831AFAD823A__INCLUDED_)
