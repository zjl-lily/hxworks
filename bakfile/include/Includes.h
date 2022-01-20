/*
*/
#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#pragma once

#include <stdlib.h>       
#include <string.h> 

typedef signed int   s32;
typedef signed short s16;
typedef signed char  s8;  
 
typedef unsigned int    u32;
typedef unsigned short  u16;
typedef unsigned char   u8; 

typedef unsigned short  UINT;
typedef unsigned int    UINT32; 



 
typedef unsigned char  BYTE; 
typedef enum {FALSE = 0, TRUE = 1} BOOL;
#define	MAX_MSG_NUM     10	//10条接收信息 
#define	MAX_MSG_LEN     255	// 一条消息的最大长度
#define	MAX_UDPMSG_LEN     1024	// 一条消息的最大长度
 

enum  COMM_MSG_STATE
{// 串口报文分解状态
	CMSG_STATE_IDLE = 0, // 无
	CMSG_STATE_HEAD,     // 有 帧头  1字节
	CMSG_STATE_MSGID,    // 有 MSGID 1字节
	CMSG_STATE_DATA,     // 数据
	CMSG_STATE_CHECK,    // 校验  正确 异或和 1字节
	CMSG_STATE_CHKERR,   // 校验 错误
	CMSG_STATE_PROC      // 已处理 
};
enum  COMM_CMD_STATE
{// 串口命令状态
	CCMD_STATE_IDLE = 0,
	CCMD_STATE_TXSQ,
	CCMD_STATE_CMD,
	CCMD_STATE_END,
};
typedef struct
{
	u8      state;
	u8      msgID;
	u8      userID;
	u8	    ackResult;
	u8	    ackNum;
	u16     cnt;
	u16     len;
	BYTE	buff[MAX_MSG_LEN+1];
}MSG_RECV_INFO;

typedef struct tagMsgList
{
	MSG_RECV_INFO	message[MAX_MSG_NUM];
	s8	front;
	s8	rear;
	s8  len;
}MSG_INFO_LIST;

#define  MAX_OUTCOUNT  18  // 最大超时次数
#define  MAX_OUTTIMES  500  // 最大超时时间 单位ms
typedef struct
{
 	COMM_CMD_STATE      state;
	u8      counts; // 发送次数
    u8      times;  // 时间
    u8      outcounts;//超时次数
	u8      bYXSQ;  // 收到允许通信申请帧？ true=收到
    u8      TXcount;
	BYTE    TXSQCmd;  // 0x5E  
}COMCMD_INFO_TYPE;

typedef struct tagMsgQuene
{  
	tagMsgList*  pQuene;
 	void  init(MSG_INFO_LIST*  plist)
	{
		pQuene = plist;
		pQuene->front = pQuene->rear = 0;
		pQuene->len = 0;
	}
	bool  empty()
	{
		if (pQuene->front == pQuene->rear)
		{
			return false;
		}
		else
			return true;
	}
	MSG_RECV_INFO*  pop()
	{// 取出 一个报文
		MSG_RECV_INFO*  pmsg = &pQuene->message[pQuene->front];
 		pQuene->front = (pQuene->front == MAX_MSG_NUM - 1) ? 0 : pQuene->front+1;
		pQuene->len--;
		return pmsg;
	}
	void  push(MSG_RECV_INFO msg)
	{// 添加一个报文
		MSG_RECV_INFO*  pNewmsg = &pQuene->message[pQuene->rear];
		pNewmsg->len = msg.len;
		pNewmsg->cnt = msg.cnt; 
		memcpy(pNewmsg->buff, msg.buff, msg.cnt);
		pNewmsg->state = CMSG_STATE_CHECK; 
		pNewmsg->msgID = msg.msgID;

		pQuene->rear = (pQuene->rear == MAX_MSG_NUM - 1) ? 0 : pQuene->rear+1;
		pQuene->len++;
	}
	void  pushinfo(BYTE* pInfo, int len)
	{// 添加报文
		MSG_RECV_INFO*  pNewmsg = &pQuene->message[pQuene->rear];  
		pNewmsg->len = len;
		pNewmsg->cnt = len; 
		memcpy(pNewmsg->buff, pInfo, len);
		pNewmsg->state = CMSG_STATE_CHECK; 
		pNewmsg->msgID = pInfo[1];

		pQuene->rear = (pQuene->rear == MAX_MSG_NUM - 1) ? 0 : pQuene->rear+1;
		len++;
	}
	MSG_RECV_INFO* front()
	{// 读取第一个报文
		MSG_RECV_INFO*  pmsg = &pQuene->message[pQuene->front];
 		return pmsg;
	}
	MSG_RECV_INFO* back()
	{// 读取最后一个报文
		MSG_RECV_INFO*  pmsg = &pQuene->message[pQuene->rear];
 		return pmsg;
	} 
	int size()
	{// 报文个数
		return pQuene->len;
	}
}MSG_INFO_QUENE;
   

typedef struct
{
	u8      state;
	u8      msgID;
	u8      userID;
	u8	    ackResult;
	u8	    ackNum;
	u16     cnt;
	u16     len;
	BYTE	buff[MAX_UDPMSG_LEN];
}UDPMSG_RECV_INFO;

typedef struct
{
	UDPMSG_RECV_INFO	message[MAX_MSG_NUM];
	s16	front;
	s16	rear;
}UDPMSG_INFO_LIST;

typedef  struct TimeStruct
{
	bool   bIsValid;//true=有效时间，false=无效
	u16    nYear;
	u16   byMonth;
	u16   byDay;

	// 时间格式 12:59:08:0158
	u16  byHour;//小时
	u16  byMinutes;//分钟
	u16  bySecond;//秒
	u16  byMSEL;// 毫秒  
	TimeStruct()
	{
		nYear = 0;
		byMonth = 0;
		byDay = 0;
		byHour = 0xFF;
		byMinutes = 0xFF;
		bySecond = 0xFF;
		byMSEL = 0;
		bIsValid = false;	
	}
	void Init()
	{
		nYear = 0;
		byMonth = 0;
		byDay = 0;
		byHour = 0xFF;
		byMinutes = 0xFF;
		bySecond = 0xFF;
		byMSEL = 0;
		bIsValid = false;	
	};
	bool  IsEmpty()
	{
		return  !bIsValid;
	}
	bool  operator == (TimeStruct  sttemp )
	{

		if (this->nYear != sttemp.nYear 
			|| this->byMonth != sttemp.byMonth
			|| this->byDay != sttemp.byDay
			|| this->byHour != sttemp.byHour 
			|| this->byMinutes != sttemp.byMinutes
			|| this->bySecond != sttemp.bySecond
			|| this->bIsValid != sttemp.bIsValid )
		{
			return false;
		}		
		return  true;
	};
	const TimeStruct& operator=(TimeStruct  sttemp)
	{
		this->bIsValid = sttemp.bIsValid;
		this->nYear = sttemp.nYear;
		this->byMonth = sttemp.byMonth;
		this->byDay = sttemp.byDay;
		this->byHour = sttemp.byHour;
		this->byMinutes = sttemp.byMinutes;
		this->bySecond = sttemp.bySecond;
		this->byMSEL = sttemp.byMSEL;
		return *this;

	} 
	UINT32 ToLong()
	{
		UINT32 ltime = this->byHour * 3600;
		ltime += this->byMinutes * 60;
		ltime += this->byMinutes;
		ltime = ltime * 10000 + this->byMSEL;
		return ltime;
	}
 
}TIMESTRUCT;
 

typedef  struct TitueStruct
{//经纬度结构
	bool  bySign;//标志位，0：东经（北纬），1：西经（南纬）
	bool  bIsLong;//true=经度，false=纬度
	bool  bValid;
	u16  uD;//度
	u16  uM;//分
	u16  uS;//0.0001分
	u16  uMicro;//0.001秒
	TitueStruct()
	{
		bySign = 0;
		bIsLong = true;
		uD = 0;
		uM = 0;
		uS = 0;
		uMicro = 0;
		bValid = false;
	}
	void Init()
	{
		bySign = 0;
		bIsLong = true;
		uD = 0;
		uM = 0;
		uS = 0;
		uMicro = 0;
		bValid = false;
	};
	bool  IsEmpty()
	{
		if (uD == 0 && uM == 0 && uS == 0)
		{
			return true;
		}
		else 
			return false;
	};

	const TitueStruct& operator=(TitueStruct  sttemp)
	{		
		this->bySign = sttemp.bySign;
		this->bIsLong = sttemp.bIsLong;
		this->uD = sttemp.uD;
		this->uM = sttemp.uM;
		this->uS = sttemp.uS;
		this->uMicro = sttemp.uMicro;
		return  *this;
	};

	double  ToFloat()
	{
		double fLong = 0;
		if (this->uD == 0 && this->uM == 0 && this->uS == 0)
		{
			return 0;
		}
		else
		{
			fLong = (this->uD + (this->uM * 10000+ this->uS )/600000.0);
		}
		if (bySign)
		{// 西经/南纬
			fLong *= -1;// 负数
		}
		return fLong;
	}
	void   FloatToStuct(double fRad, bool bIsLong)
	{
		Init();
		bValid = true;
		double fTemp = fRad;
		if (fRad < 0) 
		{
			bySign = true; fTemp *= -1;
		}
		this->bIsLong = bIsLong;
 		this->uD = (u16)fTemp;
		fTemp -= this->uD;
		fTemp = fTemp * 60;
		this->uM = (u16)fTemp;
		fTemp -= this->uM;
		this->uS = (u16)(fTemp * 10000);
	}
  

}TITUESTRUCT;
    
 

// 方便取字节中的某一位
typedef  struct  
{// 
	u8  D0:1;//  最低位
	u8  D1:1;// 
	u8  D2:1;// 
	u8  D3:1;// 
	u8  D4:1;// 
	u8  D5:1;// 
	u8  D6:1;// 
	u8  D7:1;// 
}BitField, *pBitField;

typedef struct 
{
	u8 byBCD0:4; // 低四位
	u8 byBCD1:4;  // 高四位
}BCD_BYTE,   *pBCD_BYTE;

union  unionDataAddr
{
	u8		out_u8[8];
	u16		out_u16;
	u32		out_u32;
	s16	    out_s16;
	s32		out_s32;	
	float   out_fp32;
	double  out_fp64; 
};
typedef struct
{
	BYTE       DevState;    //设备状态 0x00=正确，0x80=错误
	BitField   ZJstate;     //自检点状态，表示8个自检点的状态，0=正常，1=故障。 
// d0---d3: 串口1-4；  d4-d7: 网口连接状态
}DEVSTATE;



struct POS_INFO
{
	bool    bIsValid;
	u16 	utimeDelt;     //时间间隔s     fTimeDelt/10
	u32	    lTimes;        // h*3600+m*60+s

	double  dOreint;   // LD目标
	double  dDistan;   // LD目标
	double  dHeight;   // LD目标
	double  dSpeed;   // LD目标
	double  dCog;   // LD目标
	TIMESTRUCT  stDistTime; // LD目标的时戳，若是本机，则与本地时戳相同

	TITUESTRUCT  stLong;   // 询问方经度
	TITUESTRUCT  stLat;    // 询问方纬度
	TIMESTRUCT   stTime;   // 询问方时戳

	POS_INFO()
	{
		bIsValid = false;
		utimeDelt = 0;
		lTimes = 0;
		dOreint = 0;
		dDistan = 0;
		dHeight = 0;
		dSpeed = dCog = 0;
	}
	void Init()
	{
		bIsValid = false;
		utimeDelt = 0;
		lTimes = 0;
		dOreint = 0;
		dDistan = 0;
		dSpeed = dCog = 0;
		dHeight = 0;
		stLong.Init();
		stLat.Init();
		stTime.Init();
		stDistTime.Init();
	}
	const POS_INFO& operator=(POS_INFO  sttemp)
	{
		this->bIsValid = sttemp.bIsValid;
		this->utimeDelt = sttemp.utimeDelt;
		this->lTimes = sttemp.lTimes;
		this->stTime = sttemp.stTime;
		this->stDistTime = sttemp.stDistTime;

		this->dOreint = sttemp.dOreint;
		this->dDistan = sttemp.dDistan;
		this->stLong = sttemp.stLong;
		this->stLat = sttemp.stLat;
		
		this->dSpeed = sttemp.dSpeed;
		this->dCog = sttemp.dCog;  
		return *this;
	}
	bool  IsValid()
	{
		return bIsValid;		
	}
};//
  

//////////////////////////////////////////////////////////////////////////
extern	DEVSTATE  g_stDevState;
extern  u8    g_userDebug;




//////////////////////////////////////////////////////////////////////////


 
int     U16ToBuff(u8*  pBuff, u16 uLen);
int     S16ToBuff(u8*  pBuff, s16 sLen);
int     S32ToBuff(u8*  pBuff, s32 uLen);
int     u32ToBuff(u8*  pBuff, u32 uLen);
u16     BuffToU16(const u8*  pBuff);
u32     BuffToU32(const u8*  pBuff);
s16     BuffToS16(const u8*  pBuff);
s32     BuffToS32(const u8*  pBuff);
 
 
 // 将一个整数转换为BCD码  uByteNum=需要转换的字节数     uData=需要转换的整数  
bool  UintToBCD(u8*  pBuff, u32  uData, u16 uByteNum);


// 将BCD码的内存数据转换为 u32数据。
u32  BCDBuffToUint32(const u8*  pBuff, u16 uByteNum);

  
s16  HEXStringToBYTE(u8*  pSrc, u8* pDst, s16 nSrcLen);
 

void    InitComCmd(COMCMD_INFO_TYPE  *pcmd);


void   DebugPrintf(const char* file, const char* fun, const int line,  char* pinfo=NULL);
void   myprintf(const char * format,...);


#endif //  #ifndef _INCLUDES_H_