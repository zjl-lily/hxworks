// NetMsg.h: interface for the NetMsg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETMSG_H__0519F804_1BA0_4FFA_840C_0DF4A8ECFEC5__INCLUDED_)
#define AFX_NETMSG_H__0519F804_1BA0_4FFA_840C_0DF4A8ECFEC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string.h>
#include "uart.h"

#pragma pack(2)



#define  IP_ADDR_LEN   17
#define  UDPMSG_HEAD_LEN   16  // UDP���ĵı���ͷ����

enum  NETMSG_TYPE
{// ������Ϣ����    �������͵�
	NETMSG_NULL = 0,
                         //   ��������           ���շ�
	NETMSG_GZZT  = 0xDD,  //  ����״̬����    �鲥2��224.21.200.107��
	NETMSG_RQJG  = 0xE3,  //  ʶ��Ӧ��������������    �鲥1��224.21.200.101��
  	NETMSG_HEART = 0xE4,  //  ����״̬����    �鲥2��224.21.200.107��
	NETMSG_HALT  = 0xE5,  //  ����״̬����    �鲥2��224.21.200.107��
};

enum  NETCMD_TYPE
{// ������������    �������յ�
	NETCMD_NULL = 0,
	NETCMD_EMTR   = 0x15,  //  ���俪�� 
	NETCMD_RESET  = 0x19,  //  ȫ����λ 
	
	NETCMD_DHXX1 = 0xB0,  //  ������Ϣ1  ����1��������
	NETCMD_DHXX2 = 0xB1,  //  ������Ϣ2  ����1��������
	NETCMD_DHXX3 = 0xB2,  //  ������Ϣ3  ����1��������
	
	NETCMD_STXX  = 0xB3,  //  ʱͳ��Ϣ 1��/��  ����2��ʱͳ��

	NETCMD_ZTXW  = 0xE1,  //  ״̬ѯ��
	NETCMD_RQBW  = 0xE3,  //  ʶ��ѯ�ʣ����屨�ģ�

	NETCMD_HALT  = 0xE4,  //  �ػ���
	NETCMD_HEART = 0xE5,  //  ������

	NETCMD_RONGYU1_A1  = 0xE9,  //  ���෢�Ͳ��� ��
	NETCMD_RONGYU1_B1  = 0xEA,  //  ���෢�Ͳ��� ��
	NETCMD_RONGYU1_A2  = 0xEB,  //  ������ղ��� ��
	NETCMD_RONGYU1_B2  = 0xEC,  //  ������ղ��� ��

};

typedef struct tag_CPHEAD
{	
	UINT     length;
	UINT     spare;
	UINT32   srcAddr;
	UINT32   dstAddr;
	BYTE     sid;   // ���к�
	BYTE     ack;   // ȷ�Ϻ�
	BYTE     flag;  // ���ı�ʶ
 	BYTE     sum;   // ��Ϣ��Ԫ����
	tag_CPHEAD()
	{
		length = 0;
		spare = 0;
		srcAddr = dstAddr = 0;
		sid = ack = 0;
		flag = sum = 0;
	}
	void init()
	{
		length = 0;
		spare = 0;
		srcAddr = dstAddr = 0;
		sid = ack = 0;
		flag = sum = 0;

	}
}CPHEAD;
typedef struct tag_MPCKDATA
{
	u32      bak1;
	u32      bak2;
	u16      flag;  // �����ʶ  ������=0ʱ��Ч  bit0=ˮƽ��0=�أ�1=��  bit1=��ֱ��0=�أ�1=��
	u16      reset; // ��λ��ʶ  ������=1ʱ��Ч
	u32      ctrl;  // ������ 0=���ؿ��ƣ�1=��λ����
}MPCKDATA;
typedef struct tag_MPACKET
{	
	u16      mid;       // ��Ԫ��ʶ
	u16      length;    // ��Ԫ����
	u16      frameID;      // ��Ԫ���к� 0-65535
	u16      packetID;     // ������

	union
	{
		MPCKDATA  msg;
		BYTE   data[128];
	};
/*    BYTE     data[128]; // ��Ϣ��Ԫ���� */
	void init()
	{
		frameID = 0;
		mid = 0;
		length = 0 ;
		packetID = 0; 
    	memset(data,'\0',sizeof(data)); 
	}
}MPACKET;

typedef struct tag_DevState
{
	u16  uEmtr;
	u16  uReset;
}DEVICE_STATE;

typedef struct tag_SBSQ
{	
	UINT        mode; 
	UINT        pihao; 
	UINT32      distan;
	UINT        orient;
	UINT        course;  // ��Ժ���
	UINT        speed;  // ����ٶ�

	void init()
	{
		pihao = 0;
		mode = 0 ;
		distan = 0 ; 
		orient = 0 ;
		course = 0; 
		speed = 0; 
	}
}UDP_SBSQ;

typedef struct tag_SBJG
{	
	UINT        pihao; 
	UINT        result; 
	UINT32      bianhao;
	UINT32      height;
	UINT32      distan;
	UINT        orient;

	void init()
	{
		pihao = 0;
		result = 0 ;
		bianhao = 0 ;
		height = 0; 
		distan = 0 ;
		orient = 0; 
	}
}UDP_SBJG;
  
typedef struct
{
	u8     flag;     //  1=�������෢�Ͳ��ԣ� 2=������ղ���
	u16    counts;   //  ���Դ���  ��λ ���
	u16    delt;     //  ���Լ��  ��λ ms
}RONGYU_TEST;

 

UINT  DecodeOneNETCmd(UDPMSG_RECV_INFO  *pUDPMsg);// ���籨��

 
UINT  EncodeOneNetMsg(UINT uType,  void* pPara,  void* pMsg);


//////////////////////////////////////////////////////////////////////////

int  EncodeNet_IPAddr(BYTE*  pBuff, UINT32  uIP); // ID��ַ
int  EncodeNet_Time4(BYTE*  pInfo, TIMESTRUCT stTime);
int  EncodeNet_Pihao(BYTE*  pInfo, UINT uPihao);
int  EncodeNet_ShipID(BYTE*  pInfo, UINT32 uShipID);

//////////////////////////////////////////////////////////////////////////

 extern  RONGYU_TEST      g_stRongYu;

 extern  DEVICE_STATE      g_setdevState;
#endif // !defined(AFX_NETMSG_H__0519F804_1BA0_4FFA_840C_0DF4A8ECFEC5__INCLUDED_)
