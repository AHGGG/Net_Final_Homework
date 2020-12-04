#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <mysql/mysql.h>

#include "typess.h"
#include "userop.h"

#define PORT 12345
#define SIZE 64
#define MAXDATASIZE 200
#define BACKLOG 5

/**
 * @brief 线程摧毁时调用的函数
 * 
 * @param arg 
 */
void destr(void *arg){
	printf("destroy memory, pthread_self is %ld\n\n",pthread_self());
	free(arg);
}

/**
 * @brief 从connectfd recv一次数据
 * 
 * @param connectfd 
 * @param numbytes 
 * @param buffer 
 */
void recvdata(int connectfd, int *numbytes, char *buffer){
	if( (*numbytes = recv(connectfd,buffer,MAXDATASIZE,0)) == -1  ){
		perror("recv error.");
		exit(1);
	}
	buffer[*numbytes] = '\0';
}

/**
 * @brief 从connectfd读取一次信息然后输出
 * 
 * @param connectfd 
 * @param numbytes 
 * @param buffer 
 */
void read_once(int connectfd, int *numbytes, char *buffer){
	memset(buffer,0,sizeof(buffer));
	recvdata(connectfd,numbytes,buffer);
	printf("thread %ld recv message: %s\n",pthread_self(),buffer);
}

/**
 * @brief 写文件函数
 * 
 * @param sockfd connectfd
 * @param namebuffer 文件名
 * @return int 
 */
int write_file(int sockfd, char *namebuffer){
  int n;
  FILE *fp;
  char *filename = namebuffer;
  char buffer[SIZE];

	fp = fopen(filename, "w");
	while (1) {
		n = recv(sockfd, buffer, SIZE, 0);
		//printf("recvonce ");
    if (n <= 0){
		break;
		return 0;
	}
	fprintf(fp, "%s", buffer);
	bzero(buffer, SIZE);
	}
	fclose(fp);
	return 1;
}

/**
 * @brief 读取id 发送确认，读取pwd 发送确认。读取otherid 发送确认(可选)
 * 
 * @param connectfd 
 * @param numbytes 
 * @param idbuffer 
 * @param pwdbuffer 
 * @param otherid 
 * @param readotherid 
 */
void read_id_pwd_otherid(int connectfd, int *numbytes, char *idbuffer,char *pwdbuffer, char *otherid, int readotherid){
			if( readotherid ){
				read_once(connectfd, numbytes, idbuffer);//读id
				if( strlen(idbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

				read_once(connectfd, numbytes, pwdbuffer);//读pwd
				if( strlen(pwdbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

				read_once(connectfd, numbytes, otherid);//读otherid
				if( strlen(otherid) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去
			}else{
				read_once(connectfd, numbytes, idbuffer);//读id
				if( strlen(idbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去
				read_once(connectfd, numbytes, pwdbuffer);//读pwd
			}
}

/**
 * @brief 判断action是什么，然后调用相应的业务函数
 * 
 * @param connectfd 
 * @param numbytes 
 * @param idbuffer 
 * @param pwdbuffer 
 * @param actbuffer 
 * @param messagebuffer 
 */
void action(int connectfd, int *numbytes, char *idbuffer,char *pwdbuffer,char *actbuffer,char *messagebuffer){

	switch( atoi(actbuffer) ){
		case 1://register
		    read_id_pwd_otherid(connectfd, numbytes, idbuffer, pwdbuffer, actbuffer, 0);
			mysql_register(idbuffer,pwdbuffer);	
			if( mysql_register(idbuffer,pwdbuffer) ) {
				send(connectfd, "注册成功", strlen( "注册成功" ), 0);
				}else {
					send(connectfd, "注册失败", strlen( "注册失败" ), 0);
			}
			break;
		case 2://login
			read_id_pwd_otherid(connectfd, numbytes, idbuffer, pwdbuffer, actbuffer, 0);
			if( mysql_login(idbuffer, pwdbuffer) ) {
				send(connectfd, "登录成功", strlen( "登录成功" ), 0);
				}else {
					send(connectfd, "登录失败", strlen( "登录失败" ), 0);
			}
			break;
		case 3://del
			read_id_pwd_otherid(connectfd, numbytes, idbuffer, pwdbuffer, actbuffer, 0);
			if( mysql_del(idbuffer, pwdbuffer ) ){
				send(connectfd, "注销成功", strlen( "注销成功" ), 0);
				}else {
					send(connectfd, "注销失败", strlen( "注销失败" ), 0);
			}
			break;
		case 4://add
			read_id_pwd_otherid(connectfd,numbytes, idbuffer, pwdbuffer, actbuffer, 1);

			if( mysql_add(idbuffer, pwdbuffer, actbuffer) ) {
				send(connectfd, "添加好友成功", strlen( "添加好友成功" ), 0);
				}else {
					send(connectfd, "添加好友失败", strlen( "添加好友失败" ), 0);
			}
			break;
		case 5://有人要发信息
			read_id_pwd_otherid(connectfd,numbytes, idbuffer, pwdbuffer, actbuffer, 1);

			read_once(connectfd, numbytes, messagebuffer);//读message
			if(  mysql_send_message(idbuffer, pwdbuffer, actbuffer, messagebuffer)  ) {
				send(connectfd, "发送消息成功", strlen( "发送消息成功" ), 0);
				}else {
					send(connectfd, "发送消息失败", strlen( "发送消息失败" ), 0);
			}
			break;
		case 6://有人要发送文件
			read_once(connectfd, numbytes, messagebuffer);//读filename
			if( strlen(messagebuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去
			if( write_file(connectfd, messagebuffer ) ){
				send(connectfd, "发送文件成功", strlen( "发送文件成功" ), 0);
				}else {
					send(connectfd, "发送文件失败", strlen( "发送文件失败" ), 0);
			}
			break;
		case 7://同步消息
			read_id_pwd_otherid(connectfd, numbytes, idbuffer, pwdbuffer, actbuffer, 0);
			memset( messagebuffer, 0, sizeof(messagebuffer) );
			if( mysql_find_message_by_useid( idbuffer, messagebuffer) ) {
				send(connectfd, messagebuffer, strlen( messagebuffer ), 0);
			}else {
				send(connectfd, "查询消息失败 || 无结果", strlen( "查询消息失败 || 无结果" ), 0);
			}
			break;
		case 8://quit
			read_id_pwd_otherid(connectfd, numbytes, idbuffer, pwdbuffer, actbuffer, 0);
			if( mysql_set_stat(idbuffer, pwdbuffer, 0) ) {
					send(connectfd, "退出成功", strlen( "退出成功" ), 0);
				}else {
					send(connectfd, "退出失败", strlen( "退出失败" ), 0);
			}
			break;
		default :
			default_branch();
			break;
	}
}

/**
 * @brief 从connectfd读action，再调用action
 * 
 * @param connectfd 
 * @param numbytes 
 * @param idbuffer 
 * @param pwdbuffer 
 * @param actbuffer 
 * @param messagebuffer 
 */
void read_input(int connectfd,int *numbytes, char *idbuffer, char *pwdbuffer, char *actbuffer,char *messagebuffer){
	printf("接收action-->argv[2]\n");
	recvdata(connectfd,numbytes,actbuffer);
	printf("thread %ld recv what client want : %d\n",pthread_self(), atoi(actbuffer) );

	//接收action后，判断action是什么，然后调用相应的东西进行处理
	action(connectfd,numbytes,idbuffer,pwdbuffer,actbuffer,messagebuffer);
}

/**
 * @brief 线程创建后走start_routine，然后这里进行一些线程变量的初始化
 * 
 * @param connectfd 
 * @param client 
 */
void process_cli(int connectfd,struct sockaddr_in client){
	char sendbuffer[MAXDATASIZE];
	char pwdbuffer[MAXDATASIZE];
	char idbuffer[MAXDATASIZE];
	char actbuffer[MAXDATASIZE];
	char messagebuffer[4096];
	int numbytes = 0;
	
	read_input(connectfd,&numbytes,idbuffer,pwdbuffer,actbuffer,messagebuffer);

	//send(connectfd, actbuffer, strlen(actbuffer), 0);
}

void *start_routine(void *arg){
	ARG *info;
	info = (ARG*)arg;
	printf("This is pthread_creat server,!!!!!start_routine here ,you got a connection form %s ,and port is %d\n",inet_ntoa((info->client).sin_addr),htons((info->client).sin_port));
	process_cli(info->connectfd,info->client);
	pthread_exit(NULL);
}

int main()
{
	mysql_output_table("user");
	mysql_output_table("friend");
	mysql_output_table("message");

	pid_t pid;
	int sockfd,connectfd;
    struct sockaddr_in server, client;
    socklen_t sin_size;
	pthread_t tid;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            printf("Create socket failed.");
            exit(1);
    }
	int opt = SO_REUSEADDR;
	setsockopt(sockfd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
            perror("Bind error.");
            exit(1);
    }
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen error.");
		exit(-1);
	}
	sin_size = sizeof(client);


	while(1){
		ARG *arg;

		if ((connectfd = accept(sockfd, (struct sockaddr *)&client, &sin_size)) == -1) {
		perror("accept error.");
		exit(-1);
		}

		arg->connectfd = connectfd;
		//先取arg指向的client，再取地址，再强转为指向void指针
		memcpy((void*)&arg->client,&client,sizeof(client));

		if( (pthread_create(&tid,NULL,start_routine,(void*)arg)) ){
			printf("pthread_creat error\n");
			exit(1);
		}

	}

}