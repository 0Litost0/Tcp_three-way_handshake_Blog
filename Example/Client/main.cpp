#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include <stdarg.h>
#include <sys/errno.h>
#include <iostream>
#include <sstream>

#define PORT 17777
#define THREAD_NUM 6  //定义创建的线程数量
#define MAXLINE 1024
struct sockaddr_in stServAddr;
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

void *func(void *) 
{
    int nConnFd;
    nConnFd = socket(AF_INET,SOCK_STREAM,0);
    printf("nConnFd : %d\n",nConnFd);

    ///在没个子线程中，都会尝试与服务器连接连接，并返回结果
    if ((connect(nConnFd,(struct sockaddr *)&stServAddr,sizeof(struct sockaddr_in)) == -1))
    {
        printf("[nConnFd] Connect failed: [%s]\n", strerror(errno));
        return (void *)-1;
    }
    else
    {
	    printf("Connect succeed!\n");
        stringstream strStream;
        strStream << "[" << nConnFd << "]" << "Send Message"; 
        printf("strStream is [%s]\n", strStream.str().c_str());
        if (-1 == write(nConnFd, strStream.str().c_str(), strStream.str().size()))
        {
            printf("[nConnFd] Connect failed: [%s]\n", strerror(errno));
            return (void *)-1;
        }
        else
        {
            printf("[nConnFd] Send succeed!\n", nConnFd);
        }
        

    }

    while(1) {}
}

int main(int argc,char *argv[])
{

    memset(&stServAddr,0,sizeof(struct sockaddr_in));
    stServAddr.sin_family = AF_INET;
    stServAddr.sin_port = htons(PORT);
    inet_aton("127.0.0.1",(struct in_addr *)&stServAddr.sin_addr); 

    //创建线程并且等待线程完成
    pthread_t nPid[THREAD_NUM];
	//system("netstat -atn | grep '17777'");
	//printf("netstat -atn\n");
    for(int i = 0 ; i < THREAD_NUM; ++i)
    {
        pthread_create(&nPid[i],NULL,&func,NULL);

    }



	sleep(3);
	//system("netstat -atn | grep '17777'");
	//printf("netstat -atn\n");
    for(int i = 0 ; i < THREAD_NUM; ++i)
    {
        pthread_join(nPid[i], NULL);
    }

    return 0;
}
