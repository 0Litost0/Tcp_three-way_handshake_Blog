#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <string>
#include <stdarg.h>
#include <sys/errno.h>
#include <iostream>
#define PORT  17777    //端口号
#define BACKLOG 2     //BACKLOG大小
#define MAXLINE 1024


using namespace std;
/**
*@brief 格式化错误信息
*
*
*@param int errnoflag
*@param int error
*@param const char *fmt
*@param va_list ap
*
*@return
* 
*
*@author Litost_Cheng
*@date 2019年1月21日
*@note 新生成函数
*/
static void ErrDoit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE-1, fmt, ap);
	if (errnoflag)
		snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": errno[%d] %s",
				 error, strerror(error));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flushes all stdio output streams */
}

/**
*@brief 判断条件，打印errno并退出
*
*
*@param bool bCondition
*@param const char *fmt
*@param ...
*
*@return 
* 
*
*@author Litost_Cheng
*@date 2019年5月11日
*@note 新生成函数
*
*/
bool CondJudgeExit(bool bCondition, const char *fmt, ...)
{
    if (!bCondition)
    {
		va_list 	ap;
		va_start(ap, fmt);
		ErrDoit(1, errno, fmt, ap);
		va_end(ap);
		exit(1);
    }
    return bCondition;
}

/**
*@brief 展示连接信息
*
*
*@param bool bCondition
*@param const char *fmt
*@param ...
*
*@return void
* 
*
*@author Litost_Cheng
*@date 2019年5月11日
*@note 新生成函数
*
*/
void Display()
{
	system("netstat -atn | grep '17777' | sort -n -t : -k 2");

	printf("netstat -atn | grep '17777' | sort -n -t : -k 2\n");

	//system("lsof -nP -iTCP | grep '17777'");
	//printf("lsof -nP -iTCP | grep '17777'\n");

}

char *pCmd[5];
int main(int argc,char *argv[])
{
    int nConLen;
    int nSockFd,nConnFd;
    struct sockaddr_in stServAddr,stConnAddr;
	int nCmd = 0;
	pCmd[0] = "socket";
	pCmd[1] = "bind";
	pCmd[2] = "listen";
	pCmd[3] = "accept_once";
	pCmd[4] = "accept_times";
	
	printf("Please input the Cmd: \n");
	for(int n=0; n<5; n++)
	{
		printf("\t[%d]: [%s]\n", n, pCmd[n]);
	}
	
	std::cin >> nCmd;

	std::string strSysCmd =  "tcpdump -i lo -s 0 -w ./Tcpdump_";
	strSysCmd += pCmd[nCmd];
	strSysCmd += ".cap";
	strSysCmd += " &";
	system(strSysCmd.c_str());
	printf("[%s]\n", strSysCmd.c_str());
	do
	{
		printf("Start:");
		Display();
		//创建套接字
		CondJudgeExit(((nSockFd = socket(AF_INET,SOCK_STREAM,0)) != -1), "Create socket failed!\n");
		if (0 == nCmd)
		{
			break;
		}
		
		//为套接字绑定地址，需要注意字节序
		memset(&stServAddr,0,sizeof(struct sockaddr_in));
		stServAddr.sin_family = AF_INET;
		stServAddr.sin_port = htons(PORT);
		stServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		
		CondJudgeExit((bind(nSockFd,(struct sockaddr *)&stServAddr,sizeof(struct sockaddr_in)) != -1), "bind failed!\n");
		if (1 == nCmd)
		{
			break;
		}
		
		//设置为被动模式	
		CondJudgeExit((listen(nSockFd,BACKLOG) != -1), "listen filed!\n");
		if (2 == nCmd)
		{
			break;
		}	

		//accept once
		nConLen = sizeof(struct sockaddr_in);
		//sleep(10);                  //sleep 10s之后接受一个连接


		//该套接字默认为阻塞模式，所以，倘若没有接受的一个成功建立的连接，则会一直阻塞在这里
		accept(nSockFd,(struct sockaddr *)&stConnAddr,(socklen_t *)&nConLen);
		
		printf("I have accept one Connect: [%s], port[%d] \n", inet_ntoa(stConnAddr.sin_addr), ntohs(stConnAddr.sin_port));
		

		
		
		if (3 == nCmd)
		{
			break;
		}

		printf("Pending on [%s]\n", pCmd[nCmd]);
		while(1)
		{
			sleep(3);                  //周期性接受连接请求
			printf("I will accept one\n");
			accept(nSockFd,(struct sockaddr *)&stConnAddr,(socklen_t *)&nConLen);
			printf("I have accept one Connect: [%s], port[%d] \n", inet_ntoa(stConnAddr.sin_addr), ntohs(stConnAddr.sin_port));

			Display();
		}		
		
	}
	while(0);
	while(1)
	{
		printf("Pending on [%s]\n", pCmd[nCmd]);
		Display();
		sleep(1);
	}

    return 0;
}