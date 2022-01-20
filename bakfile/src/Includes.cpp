 
#include "../include/Includes.h"  
#include <iostream>
#include <time.h>
#include "stdio.h"
#include <stdarg.h>
#include <string.h>



DEVSTATE  g_stDevState;
u8    g_userDebug = 1;

//////////////////////////////////////////////////////////////////////////
void    InitComCmd(COMCMD_INFO_TYPE  *pcmd)
{  
	pcmd->state = CCMD_STATE_IDLE;
	pcmd->counts = 0;
	pcmd->times = 0;
	pcmd->outcounts = 0;
	pcmd->bYXSQ = 0;
	pcmd->TXcount = 0;
	pcmd->TXSQCmd = 0x5E; 
}
int  U16ToBuff(u8*  pBuff, u16 uLen)
{
	if (!pBuff )		return 2;
	unionDataAddr  DataAddr;
	DataAddr.out_u32 = uLen;
	pBuff[0] = DataAddr.out_u8[1];
	pBuff[1] = DataAddr.out_u8[0];
	return 2;
}
int  S16ToBuff(u8*  pBuff, s16 sLen)
{
	if (!pBuff )		return 2;
	unionDataAddr  DataAddr;

	DataAddr.out_s32 = sLen;
	pBuff[0] = DataAddr.out_u8[0];
	pBuff[1] = DataAddr.out_u8[1];
	return 2;
}
int  u32ToBuff(u8*  pBuff, u32 uLen)
{
	if (!pBuff )		return 4;
	unionDataAddr  DataAddr;

	DataAddr.out_u32 = uLen;
	pBuff[0] = DataAddr.out_u8[3];
	pBuff[1] = DataAddr.out_u8[2];
	pBuff[2] = DataAddr.out_u8[1];
	pBuff[3] = DataAddr.out_u8[0];
	return 4;
}
int  S32ToBuff(u8*  pBuff, s32 sLen)
{
	if (!pBuff )		return 4;
	unionDataAddr  DataAddr;

	DataAddr.out_s32 = sLen;
	pBuff[0] = DataAddr.out_u8[3];
	pBuff[1] = DataAddr.out_u8[2];
	pBuff[2] = DataAddr.out_u8[1];
	pBuff[3] = DataAddr.out_u8[0];
	return 4;
}
u16   BuffToU16(const u8*  pBuff)
{
	int i = 0, n = 2;
	unionDataAddr  DataAddr;
	for (i = 0; i < n; i++)
	{
		DataAddr.out_u8[i] = pBuff[n- i - 1];
	}
	return DataAddr.out_u16;
}
u32   BuffToU32(const u8*  pBuff)
{
	unionDataAddr  DataAddr;
	int i = 0, n = 4;
	for (i = 0; i < n; i++)
	{
		DataAddr.out_u8[i] = pBuff[n- i - 1];
	}
	return DataAddr.out_u32;

}
s16   BuffToS16(const u8*  pBuff)
{
	int i = 0, n = 2;
	unionDataAddr  DataAddr;
	for (i = 0; i < n; i++)
	{
		DataAddr.out_u8[i] = pBuff[n- i - 1];
	}
	return DataAddr.out_s16;

}
s32   BuffToS32(const u8*  pBuff)
{
	unionDataAddr  DataAddr;
	int i = 0, n = 4;
	for (i = 0; i < n; i++)
	{
		DataAddr.out_u8[i] = pBuff[n- i - 1];
	}
	return DataAddr.out_s32;
} 
 // 将一个整数转换为BCD码  uByteNum=需要转换的字节数(1---4)     uData=需要转换的整数  
bool  UintToBCD(u8*  pBuff, u32  uData, u16 uByteNum)
{

	
	return true;
}
u32 BCDBuffToUint32(const u8*  pBuff, u16 uByteNum)
{

	 
	return 0;
}
 

//////////////////////////////////////////////////////////////////////////
int  HEXStringToBYTE(u8*  pSrc, u8* pDst, int nSrcLen)
{
	u32 i;
	int n = 0;
	if (!pSrc || nSrcLen < 1)
	{
		return 0;
	}
	 
	return n;
}
 
void   DebugPrintf(const char* file, const char* fun, const int line, char* pinfo)
{
	if (!g_userDebug)
	{
		return;
	}
	if (pinfo)
	{
		printf("%s: fun=%s ln= %d:  %s \n", file, fun,line,pinfo);
	}
	else
		printf("%s: fun=%s ln= %d:  \n", file,fun, line);
}
void   myprintf(const char * format,...)
{
	if (!g_userDebug)
	{
		return;
	}
	u32 i;
	va_list args;
	static char	DebugBuff[512];
	u16 length;
	memset(DebugBuff,0,512);
	va_start(args,format);
	vsprintf((char *)DebugBuff,format,args);
	va_end(args);

	printf("%s", DebugBuff);
 
 
}