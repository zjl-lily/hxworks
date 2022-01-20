// ThreadProc.h: interface for the ThreadProc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADPROC_H__57E11712_2E57_400E_8677_37B9B439D2F6__INCLUDED_)
#define AFX_THREADPROC_H__57E11712_2E57_400E_8677_37B9B439D2F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <stdlib.h>     
#include <unistd.h>     
#include <pthread.h>
#include <fcntl.h>    
#include <stdio.h> 
#include "Includes.h"


 
enum  THREAD_NAME
{
	TH_COMRECV = 0,
	TH_COMSEND,
	TH_UDPRECV,
	TH_UDPSEND,
	TH_PROCCMD,
	TH_TIMER,
	TH_RONGYU,
	TH_NUM,
};
enum  THREAD_RUN_TYPE
{
	THTYPE_ONCE = 0,
	THTYPE_CIRCLE,
};
typedef struct ThreadInfo  
{
	THREAD_NAME         name;  // �߳�����
	THREAD_RUN_TYPE     type;
 	bool                flag;
  	u16                 delt;  // ��ʱʱ����
	s16                 counts;  // ��ʱʱ����
    pthread_t           tid;   //  �߳�IDpublic:
     
}TH_INFO_TYPE;


class ThreadProc  
{
public:
 

	ThreadProc();
	virtual ~ThreadProc();

	void         start(THREAD_NAME uT);
	void         stop(THREAD_NAME uT);
	bool         isrun(THREAD_NAME uT);

	void         startThread();
	void         stopThread(); 
	void         setTimerInterval(UINT milsecs); //��ʱ���, ��λ  100ms
	// �������
	void  startRongYu();
	void  stopRongYu(); 

protected:
	
private:
 	
 
};

#endif // !defined(AFX_THREADPROC_H__57E11712_2E57_400E_8677_37B9B439D2F6__INCLUDED_)
