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
#define  UDPMSG_HEAD_LEN   16  // UDP报文的报文头长度

enum  NETMSG_TYPE
{// 网络消息类型    本机发送的
	NETMSG_NULL = 0,
                         //   报文类型           接收方
	NETMSG_GZZT  = 0xDD,  //  工作状态报文    组播2（224.21.200.107）
	NETMSG_RQJG  = 0xE3,  //  识别应答（认清结果）报文    组播1（224.21.200.101）
  	NETMSG_HEART = 0xE4,  //  工作状态报文    组播2（224.21.200.107）
	NETMSG_HALT  = 0xE5,  //  工作状态报文    组播2（224.21.200.107）
};

enum  NETCMD_TYPE
{// 网络命令类型    本机接收的
	NETCMD_NULL = 0,
	NETCMD_EMTR   = 0x15,  //  发射开关 
	NETCMD_RESET  = 0x19,  //  全机复位 
	
	NETCMD_DHXX1 = 0xB0,  //  导航信息1  外设1（导航）
	NETCMD_DHXX2 = 0xB1,  //  导航信息2  外设1（导航）
	NETCMD_DHXX3 = 0xB2,  //  导航信息3  外设1（导航）
	
	NETCMD_STXX  = 0xB3,  //  时统信息 1次/秒  外设2（时统）

	NETCMD_ZTXW  = 0xE1,  //  状态询问
	NETCMD_RQBW  = 0xE3,  //  识别询问（认清报文）

	NETCMD_HALT  = 0xE4,  //  关机报
	NETCMD_HEART = 0xE5,  //  心跳报

	NETCMD_RONGYU1_A1  = 0xE9,  //  冗余发送测试 开
	NETCMD_RONGYU1_B1  = 0xEA,  //  冗余发送测试 关
	NETCMD_RONGYU1_A2  = 0xEB,  //  冗余接收测试 开
	NETCMD_RONGYU1_B2  = 0xEC,  //  冗余接收测试 关

};

typedef struct tag_CPHEAD
{	
	UINT     length;
	UINT     spare;
	UINT32   srcAddr;
	UINT32   dstAddr;
	BYTE     sid;   // 序列号
	BYTE     ack;   // 确认号
	BYTE     flag;  // 报文标识
 	BYTE     sum;   // 信息单元个数
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
	u16      flag;  // 发射标识  控制字=0时有效  bit0=水平：0=关，1=开  bit1=垂直：0=关，1=开
	u16      reset; // 复位标识  控制字=1时有效
	u32      ctrl;  // 控制字 0=开关控制，1=复位控制
}MPCKDATA;
typedef struct tag_MPACKET
{	
	u16      mid;       // 单元标识
	u16      length;    // 单元长度
	u16      frameID;      // 单元序列号 0-65535
	u16      packetID;     // 打包编号

	union
	{
		MPCKDATA  msg;
		BYTE   data[128];
	};
/*    BYTE     data[128]; // 信息单元内容 */
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
	UINT        course;  // 相对航向
	UINT        speed;  // 相对速度

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
	u8     flag;     //  1=正在冗余发送测试， 2=冗余接收测试
	u16    counts;   //  测试次数  单位 万次
	u16    delt;     //  测试间隔  单位 ms
}RONGYU_TEST;

 

UINT  DecodeOneNETCmd(UDPMSG_RECV_INFO  *pUDPMsg);// 网络报文

 
UINT  EncodeOneNetMsg(UINT uType,  void* pPara,  void* pMsg);


//////////////////////////////////////////////////////////////////////////

int  EncodeNet_IPAddr(BYTE*  pBuff, UINT32  uIP); // ID地址
int  EncodeNet_Time4(BYTE*  pInfo, TIMESTRUCT stTime);
int  EncodeNet_Pihao(BYTE*  pInfo, UINT uPihao);
int  EncodeNet_ShipID(BYTE*  pInfo, UINT32 uShipID);

//////////////////////////////////////////////////////////////////////////

 extern  RONGYU_TEST      g_stRongYu;

 extern  DEVICE_STATE      g_setdevState;
#endif // !defined(AFX_NETMSG_H__0519F804_1BA0_4FFA_840C_0DF4A8ECFEC5__INCLUDED_)
