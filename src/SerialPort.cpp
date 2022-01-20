//SerialPort.cpp              实现具体的串口类
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

 
// #define COM_NAME  "/dev/ttyS"     //   linux 串口名

// #ifdef VITRUALPROT           // 加上一个宏编译，如果使用虚拟串口来调试就定义这个宏，如果使用硬件来调试就不用
//     const char *COM_NAME = "/dev/pts/"; //  这个是我设备中虚拟串口的名称
// #else
//     const char *COM_NAME = "/dev/ttyS" ; //  这个是我硬件中串口的名称
// #endif
//#define COM_DEBUG
#ifdef  COM_DEBUG
#define COMNAME_VLS  "/dev/ttyS3"     //  内设 串口1 的硬件名称              COM2
#define COMNAME_HLS  "/dev/ttyS4"     //  内设 串口2 的硬件名称    导航      COM5
#define COMNAME_WLS  "/dev/ttyS5"     //  内设 串口3 的硬件名称    时统1     COM6  
#define COMNAME_RESET  "/dev/ttyS6"     //  内设 串口3 的硬件名称    时统1     COM6  
#define COMNAME_RECV  "/dev/ttyS0"     //  内设 串口1 的硬件名称              COM2
#else
#define COMNAME_VLS  "/dev/ttyS1"     //  内设 串口1 的硬件名称              COM2
#define COMNAME_HLS  "/dev/ttyS6"     //  内设 串口2 的硬件名称    导航      COM5
#define COMNAME_WLS  "/dev/ttyS5"     //  内设 串口3 的硬件名称    时统1     COM6
#define COMNAME_RESET "/dev/ttyS7"     //  内设 串口3 的硬件名称    时统1     COM6  
#define COMNAME_RECV  "/dev/ttyS0"     //  内设 串口3 的硬件名称    时统1     COM6  
 
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
构造函数，打开串口。参数WhichCom：第几个串口
*/
SerialPort::SerialPort(int WhichCom)
{
	InitMsgInfo(&m_stRecvMsg);
	m_userid = WhichCom;
//	m_stComPara.ComPort = WhichCom;
	m_fd = -1;
	switch(WhichCom)
	{
	case COMM_VS: // 收外设3、4 认清命令          内设1   ttyS1
		sprintf(m_DevName,"%s",COMNAME_VLS);
	 //   m_stComPara.ComPort = 1; 
		break;
	case COMM_HS:      // 导航模块  收外设1的导航信息  内设2   ttyS6
		sprintf(m_DevName,"%s",COMNAME_HLS);
	 //   m_stComPara.ComPort = 6; 
		break; 
	case COMM_AS:      // 收外设2的时统信息          内设3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_WLS);
	  //  m_stComPara.ComPort = 8; 
		break; 
	case COMM_RESET:      //  发送复位      内设3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_RESET);
//	    m_stComPara.ComPort = 3; 
		break; 
	case COMM_RECV:      //  串口收
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
bool SerialPort ::OpenPort(ComPara stPara) // 初始化串口
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
	case COMM_VS: // 收外设3、4 认清命令          内设1   ttyS1
		sprintf(m_DevName,"%s",COMNAME_VLS);
	    m_stComPara.ComPort = 1; 
		break;
	case COMM_HS:      // 导航模块  收外设1的导航信息  内设2   ttyS6
		sprintf(m_DevName,"%s",COMNAME_HLS);
	    m_stComPara.ComPort = 6; 
		break; 
	case COMM_AS:      // 收外设2的时统信息          内设3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_WLS);
	    m_stComPara.ComPort = 8; 
		break; 
	case COMM_RESET:      //  发送复位      内设3   ttyS8
		sprintf(m_DevName,"%s",COMNAME_RESET);
	    m_stComPara.ComPort = 8; 
		break; 
	case COMM_RECV:      //  串口收
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
/*open函数打开串口
	O_RDWR :串口可读写
	O_NOCTTY：可以告诉Linux这个程序不会成为这个端口上的“控制终端”.如果不这样做的话,
	所有的输入,比如键盘上过来的Ctrl+C中止信号等等,会影响到你的进程。 
	O_NDELAY：标志则是告诉Linux,这个程序并不关心DCD信号线的状态——也就是不关心端口另一端是否已经连接（不阻塞）。
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
 /*重新将串口设置为阻塞模式，即执行read函数时，如果没有数据就会阻塞等待，不往下执行，
 如果设置为非阻塞模式为fcntl(fd, F_SETFL, O_NDELAY)，此时执行read函数时，如果没有数据，
 则返回-1，程序继续往下执行*/
    fcntl(m_fd, F_SETFL, 0);  //非阻塞模式

 	m_bRecv = true;
	m_bSend = true;

 
 	return InitPort(baud,data,stop,Parity);

}
bool SerialPort ::setPortPara(ComPara stPara) // 初始化串口
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
初始化串口，配置串口的各种参数。
参数：BaudRate：波特率  
DataBits：数据位 
StopBits：停止位
ParityBit：校验位
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
    m_Setting.c_cflag |= CLOCAL;//控制模式, 保证程序不会成为端口的占有者
    m_Setting.c_cflag |= CREAD; //控制模式, 使能端口读取输入的数据
 

    // 设置波特率
     /*设置波特率*/ 
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

 
    // 设置数据位
    m_Setting.c_cflag &= ~CSIZE;   
    switch(DataBits)
    {
        case 6:
			m_Setting.c_cflag |= CS6 ; 
			break;  //6位数据位
        case 7:
			m_Setting.c_cflag |= CS7 ; 
			break;  //7位数据位
        case 8:
			m_Setting.c_cflag |= CS8 ;
			break;  //8位数据位
        default:
			m_Setting.c_cflag |= CS8 ;
			break;  //8位数据位
    }

    // 设置停止位
    switch(StopBits)
    {
        case 1: /*设置停止位；若停止位为1，则清除CSTOPB，若停止位为2，则激活CSTOPB*/
			m_Setting.c_cflag &= ~CSTOPB;
			break;  //1位停止位
        case 2: 
			m_Setting.c_cflag |= CSTOPB; 
			break;  //2位停止位
        default:
			m_Setting.c_cflag &= ~CSTOPB;
			break;  //1位停止位
    }

     /*设置奇偶校验位*/
    switch(ParityBit)
    {
        case 'n':
        case 'N':/*无奇偶校验*/
                m_Setting.c_cflag &= ~PARENB;  // 关闭c_cflag中的校验位使能标志PARENB）
                m_Setting.c_iflag &= ~INPCK;   // 关闭输入奇偶检测   
                break;
        case 'o':
        case 'O': /*奇校验*/
                m_Setting.c_cflag |= (PARODD | PARENB);//激活c_cflag中的校验位使能标志PARENB，同时进行奇校验
                m_Setting.c_iflag |= (INPCK | ISTRIP);  // 开启输入奇偶检测 
                break;
        case 'e':
        case 'E':
                m_Setting.c_cflag |= PARENB;//激活c_cflag中的校验位使能标志PARENB
                m_Setting.c_cflag &= ~PARODD;// 使用偶校验
                m_Setting.c_iflag |=  (INPCK | ISTRIP);// 开启输入奇偶检测 
                break;
         default: /*无奇偶校验*/
                m_Setting.c_cflag &= ~PARENB;  // 关闭c_cflag中的校验位使能标志PARENB）
                m_Setting.c_iflag &=  (INPCK | ISTRIP);   // 关闭输入奇偶检测   
                break;
    }

    m_Setting.c_oflag &= ~OPOST;// 设置为原始输出模式
    m_Setting.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // 设置为原始输入模式
    /*所谓标准输入模式是指输入是以行为单位的，可以这样理解，输入的数据最开始存储在一个缓冲区里面（但并未真正发送出去），
    可以使用Backspace或者Delete键来删除输入的字符，从而达到修改字符的目的，当按下回车键时，输入才真正的发送出去，这样终端程序才能接收到。通常情况下我们都是使用的是原始输入模式，也就是说输入的数据并不组成行。在标准输入模式下，系统每次返回的是一行数据，在原始输入模式下，系统又是怎样返回数据的呢？如果读一次就返回一个字节，那么系统开销就会很大，但在读数据的时候，我们也并不知道一次要读多少字节的数据，
    解决办法是使用c_cc数组中的VMIN和VTIME，如果已经读到了VMIN个字节的数据或者已经超过VTIME时间，系统立即返回。*/

    m_Setting.c_cc[VTIME] = 0  ; /*非规范模式读取时的超时时间；*/
    m_Setting.c_cc[VMIN] = 0; /*非规范模式读取时的最小字符数*/


    /*刷新串口数据
    TCIFLUSH:刷新收到的数据但是不读 
    TCOFLUSH:刷新写入的数据但是不传送 
    TCIOFLUSH:同时刷新收到的数据但是不读，并且刷新写入的数据但是不传送。 */
    tcflush(m_fd, TCIFLUSH);  

     /*激活配置使其生效*/
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


// 关闭串口
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
int  SerialPort::SendOneMsg(MSG_RECV_INFO *pmsg) // 将需要发送的报文 发送到串口
{
	if (pmsg->state != CMSG_STATE_CHECK || pmsg->len < 1)
	{
		return 0;
	}
	int nE = WritePort(pmsg->buff, pmsg->len);
	return 1;

}
int  SerialPort::SendMsg() // 发送报文
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
bool  SerialPort::AddSendMsg(MSG_RECV_INFO msg)// 将需要发送的报文放到  发送报文列表
 {
	 if (msg.state != CMSG_STATE_CHECK  || msg.len > MAX_MSG_LEN || msg.len < 1)
	 {
		 return false;
	 }
	 else
		m_sendQuene.push(msg);
	 
	 return true;
 }

bool  SerialPort::AddSendInfo(char* pinfo, int len) // 将需要发送的报文放到  发送报文列表
{
	if (!pinfo || len > MAX_MSG_LEN || len < 1)
	{
		return false;
	}
	else
		m_sendQuene.pushinfo((BYTE*)pinfo, len);

	return true;
} 
MSG_RECV_INFO*  SerialPort::GetOneRecvMsg() //  从收到的报文中提取一个完整报文
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
{ // 从接收到的数据中 分解一个完整报文
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
		case CMSG_STATE_IDLE:// 无
			if(chData != COMM_MSG_HEAD) 
				break;
    		pMsg->buff[0] = chData;
			pMsg->cnt = 1;
			pMsg->len = 0;
			pMsg->msgID = 0;
			pMsg->state = CMSG_STATE_HEAD;
			break;
		case CMSG_STATE_HEAD:// 有 帧头  1字节
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++;
			pMsg->msgID = chData;
			pMsg->state = CMSG_STATE_MSGID;
			break;
		case CMSG_STATE_MSGID:// 有 MSGID 1字节
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
		case CMSG_STATE_DATA:// 数据
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++; 
			if(pMsg->cnt >= pMsg->len)
			{
 				if (IsMsgVailid(pMsg))
				{// 校验成功
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
{ // 从接收到的数据中 分解一个完整报文
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
		case CMSG_STATE_IDLE:// 无
			if(chData != COMM_MSG_HEAD) 
				break;
    		pMsg->buff[0] = chData;
			pMsg->cnt = 1;
			pMsg->len = 0;
			pMsg->msgID = 0;
			pMsg->state = CMSG_STATE_HEAD;
			break;
		case CMSG_STATE_HEAD:// 有 帧头  1字节
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++;
			pMsg->msgID = chData;
			pMsg->state = CMSG_STATE_MSGID;
			break;
		case CMSG_STATE_MSGID:// 有 MSGID 1字节
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
		case CMSG_STATE_DATA:// 数据
    		pMsg->buff[pMsg->cnt] = chData;
			pMsg->cnt++; 
			if(pMsg->cnt >= pMsg->len)
			{
 				if (IsMsgVailid(pMsg))
				{// 校验成功
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
	case CMSG_STATE_IDLE:// 无
		if(chData != COMM_MSG_HEAD) 
			break;
    	pMsg->buff[0] = chData;
		pMsg->cnt = 1;
		pMsg->len = 0;
		pMsg->msgID = 0;
		pMsg->state = CMSG_STATE_HEAD;
		break;
	case CMSG_STATE_HEAD:// 有 帧头  1字节
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		pMsg->msgID = chData;
		pMsg->state = CMSG_STATE_MSGID;
		break;
	case CMSG_STATE_MSGID:// 有 MSGID 1字节
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		if(pMsg->cnt == 4)
		{// 报文长度  
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
	case CMSG_STATE_DATA:// 数据
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++; 
		if(pMsg->cnt >= pMsg->len)
		{
 			if (IsMsgVailid(pMsg))
			{// 校验成功
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
	case CMSG_STATE_IDLE:// 无
		if(chData != COMM_MSG_HEAD) 
			break;
    	pMsg->buff[0] = chData;
		pMsg->cnt = 1;
		pMsg->len = 0;
		pMsg->msgID = 0;
		pMsg->state = CMSG_STATE_HEAD;
		break;
	case CMSG_STATE_HEAD:// 有 帧头  1字节
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++;
		pMsg->msgID = chData;
		pMsg->state = CMSG_STATE_MSGID;
		break;
	case CMSG_STATE_MSGID:// 有 MSGID 1字节
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
	case CMSG_STATE_DATA:// 数据
    	pMsg->buff[pMsg->cnt] = chData;
		pMsg->cnt++; 
		if(pMsg->cnt >= pMsg->len)
		{
 			if (IsMsgVailid(pMsg))
			{// 校验成功
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
	case CCMD_STATE_IDLE:  // 发送通信申请帧
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
		{// 错误处理
			 ProcessComSendErr(COMERR_OUTTIMES);
			 InitComCmd(&m_stCmd);
		}
 		break;
	case CCMD_STATE_CMD: // 收到允许通信帧之后
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
//从串口读取数据
int SerialPort::ReadPort(void *Buff,  u16 Len)
{
    if( -1 == m_fd)
        return -1;
    int len = read(m_fd,Buff,Len);
}

// 往串口写入数据
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
//串口 信息
BYTE  SerialPort::getState()
{
	return m_state;
}
void  SerialPort::SetState(BYTE ustate)
{
	m_state = ustate;
}

