


#include <stdlib.h>     
#include <unistd.h>    
#include <sys/types.h>     
#include <sys/stat.h>       
#include <pthread.h>
#include <fcntl.h>    
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "../include/sysProc.h"
#include "../include/lxgpio.h"
 
int main(int argc, char* argv[])
{ 
	char buffer[MAX_UDPMSG_LEN]; 
	InitGpio();
	ptrSP.startDev();

	printf("input type : quit  || bye         * exit \n") ;
    printf("input type : list                 * list all cmd \n") ;
	printf("input type : RONGYU:              * rong yu test eg:RONGYU:count,delt(ms)\n") ;
// 	printf("input type : TEST: counts(*1000), delt(ms)               * send msg to test frame lost rate \n") ;
    printf("input type : udp DH xxxx          * UDP send test \n");
    printf("input type : com 1 xxxx           * COM send test \n");
    printf("input type : debug                * UDP send test \n");
//     while(1);
 	MSG_RECV_INFO  msg;
 	UDPMSG_RECV_INFO  udpmsg;
 	int nhead = 0;
	int ndatalen = 0;
	msg.state = CMSG_STATE_CHECK;
	msg.userID = COMM_VS;

	udpmsg.state = CMSG_STATE_CHECK;
	udpmsg.userID = UDP_USER_DH;

	char *pcmd = buffer;
    BYTE *pdata = NULL;
    while (ptrSP.m_bRun && (fgets(buffer, MAX_MSG_LEN, stdin) != NULL))
	{
		pdata = NULL;	
		nhead = 0;

		ndatalen = strlen(buffer);
 		if(ndatalen >=3 && (strncmp(buffer, "quit", 4) == 0 || strncmp(buffer, "bye", 3) == 0))
		{
			printf("quit InProc \r\n");
			ptrSP.stopDev();			
            exit(1);
			return 0;
		}
		else if(strncmp(buffer, "list:", 5) == 0 && ndatalen > 4)
		{

            printf("input type : quit  || bye         * exit \n") ;
            printf("input type : list                 * list all cmd \n") ;
            printf("input type : rongyu               * rong yu test eg:rongyu count delt(ms) \n") ;
            printf("input type : udp DH xxxx          * UDP send test \n");
            printf("input type : com 1 xxxx           * COM send test \n");
            printf("input type : debug                * UDP send test \n");
			
		}
		else if(strncmp(buffer, "RONGYU:", 7) == 0 && ndatalen > 10)
		{// 发送
			UINT  temp = 0;
			pcmd = buffer + 7;
			temp = atoi(pcmd);
			g_stRongYu.flag = 1;
			g_stRongYu.counts = temp;
			pcmd = strchr(pcmd,',') + 1;
			temp = atoi(pcmd);
			g_stRongYu.delt = temp;
			printf("start rong yu test :counts=%ld, delt=%dms\n", g_stRongYu.counts, g_stRongYu.delt);
			sleep(1);
			ptrSP.m_ThProc.startRongYu();
		} 
		else if(strncmp(buffer, "COM", 3) == 0 || strncmp(buffer, "com", 3) == 0)
		{// 发送到串口

			msg.state = CMSG_STATE_CHECK;
            pcmd = strchr(buffer,' ') + 1;
            UINT temp = atoi(pcmd);
 
            if(temp == 1)
            {// 发送到串口1
                msg.userID = COMM_VS;
            }
            else if(temp == 2)
            {// 发送到串口1
                msg.userID = COMM_HS;
            }
            else if(temp == 3)
            {// 发送到串口1
                msg.userID = COMM_AS;
            } 
            else 
				msg.userID = COMM_VS;

            pcmd = strchr(pcmd, ' ') + 1;
            if(pcmd == NULL)  continue;
            ndatalen = strlen(pcmd);
            if(ndatalen > MAX_MSG_LEN)
                continue;
            msg.len = ndatalen;
            msg.cnt = ndatalen;
            memcpy(msg.buff, pcmd, ndatalen);
            msg.buff[ndatalen] = '\0';

            if (ptrSP.m_pSPlist[msg.userID] != NULL)
            {
                ptrSP.m_pSPlist[msg.userID]->AddSendMsg(msg);
            }
            InitMsgInfo(&msg);
		}
		else if(strncmp(buffer, "UDP", 3) == 0 || strncmp(buffer, "udp", 3) == 0)
		{ // sendto: userID
            udpmsg.state = CMSG_STATE_CHECK;
            pcmd = strchr(buffer,' ') + 1;

            if(strncmp(pcmd, "DH", 2) == 0)
            {
                udpmsg.userID = UDP_USER_DH;
            }
            else if(strncmp(pcmd, "ST", 2) == 0)
            {
                udpmsg.userID = UDP_USER_ST;
            }
            else if(strncmp(pcmd, "WS3", 3) == 0)
            {
                udpmsg.userID = UDP_USER_WS3;
            }
            else if(strncmp(pcmd, "WS4", 3) == 0)
            {
                udpmsg.userID = UDP_USER_WS4;
            }
            else if(strncmp(pcmd, "ZB1", 3) == 0)
            {
                udpmsg.userID = UDP_USER_ZB1;
            }
            else if(strncmp(pcmd, "ZB2", 3) == 0)
            {
                udpmsg.userID = UDP_USER_ZB2;
            }
            else
                continue;

            pcmd = strchr(pcmd, ' ') + 1;
            if(pcmd == NULL)  continue;
            ndatalen = strlen(pcmd);
            if(ndatalen >= MAX_UDPMSG_LEN)
                continue;

            udpmsg.len = ndatalen;
            udpmsg.cnt = ndatalen;
            memcpy(udpmsg.buff, pcmd, ndatalen);
            udpmsg.buff[ndatalen] = '\0';
            if (ptrSP.m_pUDPSock)
            {
                ptrSP.m_pUDPSock->AddSendMsg(udpmsg);
            }
            InitUDPMsgInfo(&udpmsg);
		}
		else
			printf("cmd is error \r\n"); 
	} 
 
	return 0;
} 
