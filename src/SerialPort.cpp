//SerialPort.cpp              ʵ�־���Ĵ�����
#include <stdlib.h>     
#include <unistd.h>    
#include <sys/types.h>     
#include <sys/stat.h>       
#include <fcntl.h>        
#include <errno.h>         
#include <string.h>
#include <stdio.h>

#include "../include/SerialPort.h"
#include "../include/comMsg.h"

 
// #define COM_NAME  "/dev/ttyS"     //   linux ������

// #ifdef VITRUALPROT           // ����һ������룬���ʹ�����⴮�������ԾͶ�������꣬���ʹ��Ӳ�������ԾͲ���
//     const char *COM_NAME = "/dev/pts/"; //  ��������豸�����⴮�ڵ�����
// #else
//     const char *COM_NAME = "/dev/ttyS" ; //  �������Ӳ���д��ڵ�����
// #endif
//#define COM_DEBUG
#ifdef  COM_DEBUG
#define COMNAME_VLS  "/dev/ttyS3"     //  ���� ����1 ��Ӳ������              COM2
#define COMNAME_HLS  "/dev/ttyS4"     //  ���� ����2 ��Ӳ������    ����      COM5
#define COMNAME_WLS  "/dev/ttyS5"     //  ���� ����3 ��Ӳ������    ʱͳ1     COM6  
#define COMNAME_RESET  "/dev/ttyS6"     //  ���� ����3 ��Ӳ������    ʱͳ1     COM6  
#define COMNAME_RECV  "/dev/ttyS0"     //  ���� ����1 ��Ӳ������              COM2
#else
#define COMNAME_VLS  "/dev/ttyS1"     //  ���� ����1 ��Ӳ������              COM2
#define COMNAME_HLS  "/dev/ttyS6"     //  ���� ����2 ��Ӳ������    ����      COM5
#define COMNAME_WLS  "/dev/ttyS5"     //  ���� ����3 ��Ӳ������    ʱͳ1     COM6
#define COMNAME_RESET "/dev/ttyS7"     //  ���� ����3 ��Ӳ������    ʱͳ1     COM6  
#define COMNAME_RECV  "/dev/ttyS0"     //  ���� ����3 ��Ӳ������    ʱͳ1     COM6  
 
#endif

  
SerialPort::SerialPort()
{
 	m_fd = -1;
	m_bRecv = false;
	m_bSend = false;
	InitUART(&m_stUart);
	InitMsgQueue(&m_stRMsgList);
	InitMsgQueue(&m_stSMsgList); 
	m_state = 0x00;
	m_recvQuene.init(&m_stRMsgList);
	m_sendQuene.init(&m_stSMsgList);
	InitMsgInfo(&m_stRecvMsg);
	InitComCmd(&m_stCmd);
}

/*
���캯�����򿪴��ڡ�����WhichCom���ڼ�������
*/
SerialPort::SerialPort(int WhichCom)
{
	InitMsgInfo(&m_stRecvMsg);
	m_userid = WhichCom;
//	m_stComPara.ComPort = WhichCom;
	m_fd = -1;
	switch(WhichCom)
	{
	case COMM_VS: // ������3��4 ��������          ����1   ttyS1
		sprintf(m_DevName,"%s",COMNAME_VLS);
	 //   m_stComPara.ComPort = 1; 
		break;
	case COMM_HS:      // ����ģ��  ������1�ĵ�����Ϣ  ����2   ttyS6
		sprintf(m_DevName,"%s",COMNAME_HLS);
	 //   m_stComPara.ComPort = 6; 
		break; 
	case COMM_AS:      // ������2��ʱͳ��Ϣ          ����3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_WLS);
	  //  m_stComPara.ComPort = 8; 
		break; 
	case COMM_RESET:      //  ���͸�λ      ����3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_RESET);
//	    m_stComPara.ComPort = 3; 
		break; 
	case COMM_RECV:      //  ������
		sprintf(m_DevName,"%s",COMNAME_RECV);
//	    m_stComPara.ComPort = 4; 
		break;  
	default:
    	sprintf(m_DevName,"%s",COMNAME_VLS);
//	    m_stComPara.ComPort = 7; 
		break;
	} 
	
	m_bRecv = false;
	m_bSend = false;

	m_state = 0x00;
	InitUART(&m_stUart);
	InitMsgQueue(&m_stRMsgList);
	InitMsgQueue(&m_stSMsgList); 

	m_recvQuene.init(&m_stRMsgList);
	m_sendQuene.init(&m_stSMsgList);

}
bool SerialPort ::OpenPort(ComPara stPara) // ��ʼ������
{ 
	m_stComPara = stPara;
//	int ComPort = m_stComPara.ComPort;
	int baud = m_stComPara.baud;
	int data = m_stComPara.data;
	int stop = m_stComPara.stop;
	char Parity = m_stComPara.Parity;
	/*
	
	switch(ComPort)
	{
	case COMM_VS: // ������3��4 ��������          ����1   ttyS1
		sprintf(m_DevName,"%s",COMNAME_VLS);
	    m_stComPara.ComPort = 1; 
		break;
	case COMM_HS:      // ����ģ��  ������1�ĵ�����Ϣ  ����2   ttyS6
		sprintf(m_DevName,"%s",COMNAME_HLS);
	    m_stComPara.ComPort = 6; 
		break; 
	case COMM_AS:      // ������2��ʱͳ��Ϣ          ����3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_WLS);
	    m_stComPara.ComPort = 8; 
		break; 
	case COMM_RESET:      //  ���͸�λ      ����3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_RESET);
	    m_stComPara.ComPort = 8; 
		break; 
	case COMM_RECV:      //  ������
		sprintf(m_DevName,"%s",COMNAME_RECV);
	    m_stComPara.ComPort = 8; 
		break; 
	default:
    	sprintf(m_DevName,"%s",COMNAME_VLS);
	    m_stComPara.ComPort = 7; 
		break;
	} 
	*/
/*	sprintf(m_DevName,"%s%d",COM_NAME, ComPort);*/
/*open�����򿪴���
	O_RDWR :���ڿɶ�д
	O_NOCTTY�����Ը���Linux������򲻻��Ϊ����˿��ϵġ������նˡ�.������������Ļ�,
	���е�����,��������Ϲ�����Ctrl+C��ֹ�źŵȵ�,��Ӱ�쵽��Ľ��̡� 
	O_NDELAY����־���Ǹ���Linux,������򲢲�����DCD�ź��ߵ�״̬����Ҳ���ǲ����Ķ˿���һ���Ƿ��Ѿ����ӣ�����������
*/
    m_fd = open( m_DevName, O_RDWR | O_NOCTTY |O_NDELAY);

    if(m_fd < 0)
    {
        m_fd = -1;    
		m_state = 0x80;

        printf("Can't Open the %s device.\n", m_DevName);
        return false;
    }
    else 
        printf("open the %s device OK. \r\n", m_DevName);

    bzero(&m_Setting, sizeof(m_Setting));   
 /*���½���������Ϊ����ģʽ����ִ��read����ʱ�����û�����ݾͻ������ȴ���������ִ�У�
 �������Ϊ������ģʽΪfcntl(fd, F_SETFL, O_NDELAY)����ʱִ��read����ʱ�����û�����ݣ�
 �򷵻�-1�������������ִ��*/
    fcntl(m_fd, F_SETFL, 0);  //������ģʽ

 	m_bRecv = true;
	m_bSend = true;

 
 	return InitPort(baud,data,stop,Parity);

}
bool SerialPort ::setPortPara(ComPara stPara) // ��ʼ������
{
	m_stComPara = stPara;
	int ComPort = m_stComPara.ComPort;
	int baud = m_stComPara.baud;
	int data = m_stComPara.data;
	int stop = m_stComPara.stop;
	char Parity = m_stComPara.Parity;

	m_bRecv = true;
	m_bSend = true;


 	return InitPort(baud,data,stop,Parity);

}
/*
��ʼ�����ڣ����ô��ڵĸ��ֲ�����
������BaudRate��������  
DataBits������λ 
StopBits��ֹͣλ
ParityBit��У��λ
*/
bool SerialPort ::InitPort(int BaudRate,int DataBits,int StopBits,char ParityBit)
{
    if( -1 == m_fd)
        return false;

    if( 0!= tcgetattr (m_fd, &m_Setting))
    {
        printf("InitPort tcgetattr() line:%d failed\n",__LINE__);
        return false;
    }
    bzero(&m_Setting,sizeof(m_Setting));
    m_Setting.c_cflag |= CLOCAL;//����ģʽ, ��֤���򲻻��Ϊ�˿ڵ�ռ����
    m_Setting.c_cflag |= CREAD; //����ģʽ, ʹ�ܶ˿ڶ�ȡ���������
 

    // ���ò�����
     /*���ò�����*/ 
	switch( BaudRate) 
     { 
		 case 2400: 
		  cfsetispeed(&m_Setting, B2400); 
		  cfsetospeed(&m_Setting, B2400); 
		  break; 
		 case 4800: 
		  cfsetispeed(&m_Setting, B4800); 
		  cfsetospeed(&m_Setting, B4800); 
		  break; 
		 case 9600: 
		  cfsetispeed(&m_Setting, B9600); 
		  cfsetospeed(&m_Setting, B9600); 
		  break; 
		 case 115200: 
		  cfsetispeed(&m_Setting, B115200); 
		  cfsetospeed(&m_Setting, B115200); 
		  break; 
		 case 460800: 
		  cfsetispeed(&m_Setting, B460800); 
		  cfsetospeed(&m_Setting, B460800); 
		  break; 
		 default: 
		  cfsetispeed(&m_Setting, B115200); 
		  cfsetospeed(&m_Setting, B115200); 
		 break; 
     } 

 
    // ��������λ
    m_Setting.c_cflag &= ~CSIZE;   
    switch(DataBits)
    {
        case 6:
			m_Setting.c_cflag |= CS6 ; 
			break;  //6λ����λ
        case 7:
			m_Setting.c_cflag |= CS7 ; 
			break;  //7λ����λ
        case 8:
			m_Setting.c_cflag |= CS8 ;
			break;  //8λ����λ
        default:
			m_Setting.c_cflag |= CS8 ;
			break;  //8λ����λ
    }

    // ����ֹͣλ
    switch(StopBits)
    {
        case 1: /*����ֹͣλ����ֹͣλΪ1�������CSTOPB����ֹͣλΪ2���򼤻�CSTOPB*/
			m_Setting.c_cflag &= ~CSTOPB;
			break;  //1λֹͣλ
        case 2: 
			m_Setting.c_cflag |= CSTOPB; 
			break;  //2λֹͣλ
        default:
			m_Setting.c_cflag &= ~CSTOPB;
			break;  //1λֹͣλ
    }

     /*������żУ��λ*/
    switch(ParityBit)
    {
        case 'n':
        case 'N':/*����żУ��*/
                m_Setting.c_cflag &= ~PARENB;  // �ر�c_cflag�е�У��λʹ�ܱ�־PARENB��
                m_Setting.c_iflag &= ~INPCK;   // �ر�������ż���   
                break;
        case 'o':
        case 'O': /*��У��*/
                m_Setting.c_cflag |= (PARODD | PARENB);//����c_cflag�е�У��λʹ�ܱ�־PARENB��ͬʱ������У��
                m_Setting.c_iflag |= (INPCK | ISTRIP);  // ����������ż��� 
                break;
        case 'e':
        case 'E':
                m_Setting.c_cflag |= PARENB;//����c_cflag�е�У��λʹ�ܱ�־PARENB
                m_Setting.c_cflag &= ~PARODD;// ʹ��żУ��
                m_Setting.c_iflag |=  (INPCK | ISTRIP);// ����������ż��� 
                break;
         default: /*����żУ��*/
                m_Setting.c_cflag &= ~PARENB;  // �ر�c_cflag�е�У��λʹ�ܱ�־PARENB��
                m_Setting.c_iflag &=  (INPCK | ISTRIP);   // �ر�������ż���   
                break;
    }

    m_Setting.c_oflag &= ~OPOST;// ����Ϊԭʼ���ģʽ
    m_Setting.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // ����Ϊԭʼ����ģʽ
    /*��ν��׼����ģʽ��ָ����������Ϊ��λ�ģ�����������⣬����������ʼ�洢��һ�����������棨����δ�������ͳ�ȥ����
    ����ʹ��Backspace����Delete����ɾ��������ַ����Ӷ��ﵽ�޸��ַ���Ŀ�ģ������»س���ʱ������������ķ��ͳ�ȥ�������ն˳�����ܽ��յ���ͨ����������Ƕ���ʹ�õ���ԭʼ����ģʽ��Ҳ����˵��������ݲ�������С��ڱ�׼����ģʽ�£�ϵͳÿ�η��ص���һ�����ݣ���ԭʼ����ģʽ�£�ϵͳ���������������ݵ��أ������һ�ξͷ���һ���ֽڣ���ôϵͳ�����ͻ�ܴ󣬵��ڶ����ݵ�ʱ������Ҳ����֪��һ��Ҫ�������ֽڵ����ݣ�
    ����취��ʹ��c_cc�����е�VMIN��VTIME������Ѿ�������VMIN���ֽڵ����ݻ����Ѿ�����VTIMEʱ�䣬ϵͳ�������ء�*/

    m_Setting.c_cc[VTIME] = 0  ; /*�ǹ淶ģʽ��ȡʱ�ĳ�ʱʱ�䣻*/
    m_Setting.c_cc[VMIN] = 0; /*�ǹ淶ģʽ��ȡʱ����С�ַ���*/


    /*ˢ�´�������
    TCIFLUSH:ˢ���յ������ݵ��ǲ��� 
    TCOFLUSH:ˢ��д������ݵ��ǲ����� 
    TCIOFLUSH:ͬʱˢ���յ������ݵ��ǲ���������ˢ��д������ݵ��ǲ����͡� */
    tcflush(m_fd, TCIFLUSH);  

     /*��������ʹ����Ч*/
    if( 0 != tcsetattr(m_fd,TCSANOW,&m_Setting))
    {
        printf("InitSerialPort %s failed. line = %d \n", m_DevName, __LINE__);
        return false;
    }
    else
		printf("InitSerailPort %s successed. line = %d \r\n", m_DevName, __LINE__);  
  
    return true;
}

SerialPort:: ~SerialPort()
{ 
 	m_bRecv = false;
	m_bSend = false;
	ClosePort();
	
}
int  SerialPort::getFD()
{
	return m_fd;
}


// �رմ���
bool SerialPort::ClosePort()
{
    if( -1 == m_fd)
        return false;

    close(m_fd);
    m_fd = -1;
	m_bRecv = false;
	m_bSend = false;

    return true;
}
int  SerialPort::SendOneMsg(MSG_RECV_INFO *pmsg) // ����Ҫ���͵ı��� ���͵�����
{
	if (pmsg->state != CMSG_STATE_CHECK || pmsg->len < 1)
	{
		return 0;
	}
	int nE = WritePort(pmsg->buff, pmsg->len);
	return 1;

}
int  SerialPort::SendMsg() // ���ͱ���
{
	int n = 0;
	int len = 0;
	MSG_RECV_INFO*  pMsg = NULL;
	for (n = 0; n < MAX_MSG_NUM; n++)
	{
		pMsg = &m_stSMsgList.message[n];
		if (pMsg->state == CMSG_STATE_CHECK)
		{
			len = WritePort(pMsg->buff, pMsg->len);
			InitMsgInfo(pMsg);
		} 
	} 
	return 1;
}
bool  SerialPort::AddSendMsg(MSG_RECV_INFO msg)// ����Ҫ���͵ı��ķŵ�  ���ͱ����б�
 {
	 if (msg.state != CMSG_STATE_CHECK  || msg.len > MAX_MSG_LEN || msg.len < 1)
	 {
		 return false;
	 }
	 else
		m_sendQuene.push(msg);
	 
	 return true;
 }

bool  SerialPort::AddSendInfo(char* pinfo, int len) // ����Ҫ���͵ı��ķŵ�  ���ͱ����б�
{
	if (!pinfo || len > MAX_MSG_LEN || len < 1)
	{
		return false;
	}
	else
		m_sendQuene.pushinfo((BYTE*)pinfo, len);

	return true;
} 
MSG_RECV_INFO*  SerialPort::GetOneRecvMsg() //  ���յ��ı�������ȡһ����������
{ 
	return  m_recvQuene.pop();
} 

int SerialPort::RecvMsg()
{
	int   nsus = 0;
	MSG_RECV_INFO  *pMsg = NULL;
  	int len = ReadPort(m_stUart.rx_buf, UART_BUFF_SIZE);    
	if (len > 0)
	{ 
		m_stUart.rx_rear = len;
		m_stUart.rx_front = 0;
		m_stUart.rx_buf[len] = '\0'; 
 		nsus = FetchOneMsg(m_stUart.rx_buf,len); 
	} 
	return nsus;
}
// 


int SerialPort::FetchOneMsg(BYTE* pinfo, u16 len)
{ // �ӽ��յ��������� �ֽ�һ����������
	if (!pinfo || len < 1)
	{
		return 0;
	}
	int     nmsg = 0; 
	BYTE    chData;  
	MSG_RECV_INFO*  pMsg = &m_stRecvMsg;
	for (int i = 0; i < len; i++)
	{ 
		chData = pinfo[i];
   		switch(pMsg->state)
		{
		case CMSG_STATE_IDLE:// ��
			if(chData != COMM_MSG_HEAD) 
				break;
    		pMsg->buff[0] = chData;
			pMsg->cnt = 1;
			pMsg->len = 0;
			pMsg->msgID = 0;
			pMsg->state = CMSG_STATE_HEAD;
			break;
		case CMSG_STATE_HEAD:// �� ֡ͷ  1�ֽ�
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++;
			pMsg->msgID = chData;
			pMsg->state = CMSG_STATE_MSGID;
			break;
		case CMSG_STATE_MSGID:// �� MSGID 1�ֽ�
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++;
			if(pMsg->cnt == 4)
			{
				pMsg->len = chData; 
				if (pMsg->len >= MAX_MSG_LEN)
				{
					InitMsgInfo(pMsg);
					break;
				}
				else
					pMsg->state = CMSG_STATE_DATA;
			}
			break;
		case CMSG_STATE_DATA:// ����
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++; 
			if(pMsg->cnt >= pMsg->len)
			{
 				if (IsMsgVailid(pMsg))
				{// У��ɹ�
					pMsg->state = CMSG_STATE_CHECK; 
					nmsg++;
					
					m_stRecvMsg.userID = m_userid;
					m_recvQuene.push(m_stRecvMsg);
				}
				else
				{
	// 				pMsg->state = CMSG_STATE_CHKERR; 
					pMsg->state = CMSG_STATE_CHECK; 
					nmsg++;
					m_stRecvMsg.userID = m_userid;
					m_recvQuene.push(m_stRecvMsg);
				}  

		    	InitMsgInfo(&m_stRecvMsg);
			} 
			break;
		default:
			break;
		} 
	}
	return nmsg;
}

 /*

int SerialPort::FetchOneMsg(BYTE* pinfo, u16 len)
{ // �ӽ��յ��������� �ֽ�һ����������
	if (!pinfo || len < 1)
	{
		return 0;
	}
	int     nmsg = 0; 
	BYTE    chData;  
	MSG_RECV_INFO*  pMsg = &m_stRecvMsg;
	for (int i = 0; i < len; i++)
	{ 
		chData = pinfo[i];
   		switch(pMsg->state)
		{
		case CMSG_STATE_IDLE:// ��
			if(chData != COMM_MSG_HEAD) 
				break;
    		pMsg->buff[0] = chData;
			pMsg->cnt = 1;
			pMsg->len = 0;
			pMsg->msgID = 0;
			pMsg->state = CMSG_STATE_HEAD;
			break;
		case CMSG_STATE_HEAD:// �� ֡ͷ  1�ֽ�
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++;
			pMsg->msgID = chData;
			pMsg->state = CMSG_STATE_MSGID;
			break;
		case CMSG_STATE_MSGID:// �� MSGID 1�ֽ�
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++;
			pMsg->len = chData; 
			if (pMsg->len >= MAX_MSG_LEN)
			{
				InitMsgInfo(pMsg);
				break;
			}
			else
				pMsg->state = CMSG_STATE_DATA;
			break;
		case CMSG_STATE_DATA:// ����
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++; 
			if(pMsg->cnt >= pMsg->len)
			{
 				if (IsMsgVailid(pMsg))
				{// У��ɹ�
					pMsg->state = CMSG_STATE_CHECK; 
					nmsg++;
					
					m_stRecvMsg.userID = m_userid;
					m_recvQuene.push(m_stRecvMsg);
				}
				else
				{
	// 				pMsg->state = CMSG_STATE_CHKERR; 
					pMsg->state = CMSG_STATE_CHECK; 
					nmsg++;
					m_stRecvMsg.userID = m_userid;
					m_recvQuene.push(m_stRecvMsg);
				}  

		    	InitMsgInfo(&m_stRecvMsg);
			} 
			break;
		default:
			break;
		} 
	}
	return nmsg;
}
*/

int SerialPort::RecvOneComFrame(BYTE chData, MSG_RECV_INFO *pMsg)
{
	int     nmsg = 0;
	int     rx_num;
	BYTE    info; 
	BYTE	check = 0x00; 
 	switch(pMsg->state)
	{
	case CMSG_STATE_IDLE:// ��
		if(chData != COMM_MSG_HEAD) 
			break;
    	pMsg->buff[0] = chData;
		pMsg->cnt = 1;
		pMsg->len = 0;
		pMsg->msgID = 0;
		pMsg->state = CMSG_STATE_HEAD;
		break;
	case CMSG_STATE_HEAD:// �� ֡ͷ  1�ֽ�
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		pMsg->msgID = chData;
		pMsg->state = CMSG_STATE_MSGID;
		break;
	case CMSG_STATE_MSGID:// �� MSGID 1�ֽ�
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		if(pMsg->cnt == 4)
		{// ���ĳ���  
			pMsg->len = chData;  
			if (pMsg->len >= MAX_MSG_LEN)
			{
				InitMsgInfo(pMsg);
				break;
			}
			else
				pMsg->state = CMSG_STATE_DATA;
		}
 		break;
	case CMSG_STATE_DATA:// ����
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++; 
		if(pMsg->cnt >= pMsg->len)
		{
 			if (IsMsgVailid(pMsg))
			{// У��ɹ�
				pMsg->state = CMSG_STATE_CHECK; 
				nmsg++;
			}
			else
			{
// 				pMsg->state = CMSG_STATE_CHKERR; 
				pMsg->state = CMSG_STATE_CHECK; 
				nmsg++;
			} 
			m_recvQuene.push(*pMsg); 
		}
		break;
	default:
		break;
	}
 
	return nmsg;
}
/*

int SerialPort::RecvOneComFrame(BYTE chData, MSG_RECV_INFO *pMsg)
{
	int     nmsg = 0;
	int     rx_num;
	BYTE    info; 
	BYTE	check = 0x00; 
 	switch(pMsg->state)
	{
	case CMSG_STATE_IDLE:// ��
		if(chData != COMM_MSG_HEAD) 
			break;
    	pMsg->buff[0] = chData;
		pMsg->cnt = 1;
		pMsg->len = 0;
		pMsg->msgID = 0;
		pMsg->state = CMSG_STATE_HEAD;
		break;
	case CMSG_STATE_HEAD:// �� ֡ͷ  1�ֽ�
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		pMsg->msgID = chData;
		pMsg->state = CMSG_STATE_MSGID;
		break;
	case CMSG_STATE_MSGID:// �� MSGID 1�ֽ�
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		pMsg->len = chData; 
		if (pMsg->len >= MAX_MSG_LEN)
		{
			InitMsgInfo(pMsg);
			break;
		}
		else
			pMsg->state = CMSG_STATE_DATA;
		break;
	case CMSG_STATE_DATA:// ����
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++; 
		if(pMsg->cnt >= pMsg->len)
		{
 			if (IsMsgVailid(pMsg))
			{// У��ɹ�
				pMsg->state = CMSG_STATE_CHECK; 
				nmsg++;
			}
			else
			{
// 				pMsg->state = CMSG_STATE_CHKERR; 
				pMsg->state = CMSG_STATE_CHECK; 
				nmsg++;
			} 
			m_recvQuene.push(*pMsg); 
		}
		break;
	default:
		break;
	}
 
	return nmsg;
}
*/
void SerialPort::ProcessComSendErr(COMSEND_ERR  uerr)
{
	switch(uerr)
	{
	case COMERR_OUTTIMES:
		m_sendQuene.pop();
 		printf("COMERR_OUTTIMES: \n");
		break;
	case COMERR_TXOUT:
		m_sendQuene.pop();
 		printf("CCMD_STATE_CMD:  \n");
		break;
	default:
		break;
	}

}
void SerialPort::ProcessCom_SendCmd(u8 uid)
{
	if (m_sendQuene.size() < 1)
		return;

	MSG_RECV_INFO*  pMsg = NULL;
	pMsg = m_sendQuene.front();
 	switch(uid)
	{
	case COMCMD_MLHC: 
		InitComCmd(&m_stCmd);
		PrintHex(pMsg);
		m_sendQuene.pop();
		InitMsgInfo(pMsg);
		myprintf("%s: %s %d  %s \n", __FILE__, __FUNCTION__, __LINE__, "COMCMD_MLHC");  
		return;
 		break;
	case COMCMD_YXTX:
		m_stCmd.state = CCMD_STATE_CMD;
		m_stCmd.outcounts = 0;
		m_stCmd.counts = 0;
		m_stCmd.times = 0;
		m_stCmd.TXcount++;
		SendOneMsg(pMsg);
		PrintHex(pMsg);
		myprintf("%s: %s %d  %s \n", __FILE__, __FUNCTION__, __LINE__, "COMCMD_YXTX");   
		return;
		break;
	default:
		break;
	}

	switch(m_stCmd.state)
	{
	case CCMD_STATE_IDLE:  // ����ͨ������֡
		m_stCmd.TXSQCmd = 0x5E;
 		write(m_fd,&m_stCmd.TXSQCmd,1);   
		m_stCmd.outcounts = 1; 
		m_stCmd.state = CCMD_STATE_TXSQ;
		myprintf("%s: %s %d  %s \n", __FILE__, __FUNCTION__, __LINE__, "CCMD_STATE_IDLE");   
		break;
	case CCMD_STATE_TXSQ:
		m_stCmd.times++; // 1=100ms
		if(m_stCmd.times == 5)
		{// 500ms
			m_stCmd.times = 0;
			m_stCmd.outcounts++;
			myprintf("CCMD_STATE_IDLE: times = %d, outcounts = %d \n", m_stCmd.times, m_stCmd.outcounts);
		}
		if (m_stCmd.outcounts >= 18)
		{// ������
			 ProcessComSendErr(COMERR_OUTTIMES);
			 InitComCmd(&m_stCmd);
		}
 		break;
	case CCMD_STATE_CMD: // �յ�����ͨ��֮֡��
		m_stCmd.times++;
		if(m_stCmd.times == 5)
		{
			m_stCmd.TXcount++;
			if (m_stCmd.TXcount > 2)
			{
				 ProcessComSendErr(COMERR_TXOUT);
				 m_sendQuene.pop();
				 InitComCmd(&m_stCmd);
			}
			else
			{
				m_stCmd.times = 0;
				write(m_fd,&m_stCmd.TXSQCmd,1);   
				m_stCmd.state = CCMD_STATE_TXSQ;
			}
		}
 		myprintf("CCMD_STATE_CMD: times = %d, TXcount = %d \n", m_stCmd.times, m_stCmd.TXcount);
		break;
	default:
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
//�Ӵ��ڶ�ȡ����
int SerialPort::ReadPort(void *Buff,  u16 Len)
{
    if( -1 == m_fd)
        return -1;
    int len = read(m_fd,Buff,Len);
}

// ������д������
int SerialPort::WritePort(void *Buff, u16 Len)
{    
    if( -1 == m_fd)
        return -1;
    return write(m_fd,Buff,Len);   
}
 
void     SerialPort::setcompara(ComPara stpara)
{
	m_stComPara = stpara;
}
ComPara  SerialPort::getcompara()
{
	return m_stComPara;
}
//////////////////////////////////////////////////////////////////////////
//���� ��Ϣ
BYTE  SerialPort::getState()
{
	return m_state;
}
void  SerialPort::SetState(BYTE ustate)
{
	m_state = ustate;
}

