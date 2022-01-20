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
#define	MAX_MSG_NUM     10	//10��������Ϣ 
#define	MAX_MSG_LEN     255	// һ����Ϣ����󳤶�
#define	MAX_UDPMSG_LEN     1024	// һ����Ϣ����󳤶�
 

enum  COMM_MSG_STATE
{// ���ڱ��ķֽ�״̬
	CMSG_STATE_IDLE = 0, // ��
	CMSG_STATE_HEAD,     // �� ֡ͷ  1�ֽ�
	CMSG_STATE_MSGID,    // �� MSGID 1�ֽ�
	CMSG_STATE_DATA,     // ����
	CMSG_STATE_CHECK,    // У��  ��ȷ ���� 1�ֽ�
	CMSG_STATE_CHKERR,   // У�� ����
	CMSG_STATE_PROC      // �Ѵ��� 
};
enum  COMM_CMD_STATE
{// ��������״̬
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

#define  MAX_OUTCOUNT  18  // ���ʱ����
#define  MAX_OUTTIMES  500  // ���ʱʱ�� ��λms
typedef struct
{
 	COMM_CMD_STATE      state;
	u8      counts; // ���ʹ���
    u8      times;  // ʱ��
    u8      outcounts;//��ʱ����
	u8      bYXSQ;  // �յ�����ͨ������֡�� true=�յ�
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
	{// ȡ�� һ������
		MSG_RECV_INFO*  pmsg = &pQuene->message[pQuene->front];
 		pQuene->front = (pQuene->front == MAX_MSG_NUM - 1) ? 0 : pQuene->front+1;
		pQuene->len--;
		return pmsg;
	}
	void  push(MSG_RECV_INFO msg)
	{// ���һ������
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
	{// ��ӱ���
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
	{// ��ȡ��һ������
		MSG_RECV_INFO*  pmsg = &pQuene->message[pQuene->front];
 		return pmsg;
	}
	MSG_RECV_INFO* back()
	{// ��ȡ���һ������
		MSG_RECV_INFO*  pmsg = &pQuene->message[pQuene->rear];
 		return pmsg;
	} 
	int size()
	{// ���ĸ���
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
	bool   bIsValid;//true=��Чʱ�䣬false=��Ч
	u16    nYear;
	u16   byMonth;
	u16   byDay;

	// ʱ���ʽ 12:59:08:0158
	u16  byHour;//Сʱ
	u16  byMinutes;//����
	u16  bySecond;//��
	u16  byMSEL;// ����  
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
{//��γ�Ƚṹ
	bool  bySign;//��־λ��0����������γ����1����������γ��
	bool  bIsLong;//true=���ȣ�false=γ��
	bool  bValid;
	u16  uD;//��
	u16  uM;//��
	u16  uS;//0.0001��
	u16  uMicro;//0.001��
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
		{// ����/��γ
			fLong *= -1;// ����
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
    
 

// ����ȡ�ֽ��е�ĳһλ
typedef  struct  
{// 
	u8  D0:1;//  ���λ
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
	u8 byBCD0:4; // ����λ
	u8 byBCD1:4;  // ����λ
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
	BYTE       DevState;    //�豸״̬ 0x00=��ȷ��0x80=����
	BitField   ZJstate;     //�Լ��״̬����ʾ8���Լ���״̬��0=������1=���ϡ� 
// d0---d3: ����1-4��  d4-d7: ��������״̬
}DEVSTATE;



struct POS_INFO
{
	bool    bIsValid;
	u16 	utimeDelt;     //ʱ����s     fTimeDelt/10
	u32	    lTimes;        // h*3600+m*60+s

	double  dOreint;   // LDĿ��
	double  dDistan;   // LDĿ��
	double  dHeight;   // LDĿ��
	double  dSpeed;   // LDĿ��
	double  dCog;   // LDĿ��
	TIMESTRUCT  stDistTime; // LDĿ���ʱ�������Ǳ��������뱾��ʱ����ͬ

	TITUESTRUCT  stLong;   // ѯ�ʷ�����
	TITUESTRUCT  stLat;    // ѯ�ʷ�γ��
	TIMESTRUCT   stTime;   // ѯ�ʷ�ʱ��

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
 
 
 // ��һ������ת��ΪBCD��  uByteNum=��Ҫת�����ֽ���     uData=��Ҫת��������  
bool  UintToBCD(u8*  pBuff, u32  uData, u16 uByteNum);


// ��BCD����ڴ�����ת��Ϊ u32���ݡ�
u32  BCDBuffToUint32(const u8*  pBuff, u16 uByteNum);

  
s16  HEXStringToBYTE(u8*  pSrc, u8* pDst, s16 nSrcLen);
 

void    InitComCmd(COMCMD_INFO_TYPE  *pcmd);


void   DebugPrintf(const char* file, const char* fun, const int line,  char* pinfo=NULL);
void   myprintf(const char * format,...);


#endif //  #ifndef _INCLUDES_H_