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

#include "typess.h"
#include "userop.h"

#define PORT 12345
#define MAXDATASIZE 100
#define BACKLOG 5

static pthread_key_t Key;
int err;
static pthread_once_t  once = PTHREAD_ONCE_INIT;

void destr(void *arg){
	printf("destroy memory, pthread_self is %ld\n\n",pthread_self());
	free(arg);
}

void recvdata(int connectfd, int *numbytes, char *buffer){
	if( (*numbytes = recv(connectfd,buffer,MAXDATASIZE,0)) == -1  ){
		perror("recv error.");
		exit(1);
	}
	buffer[*numbytes] = '\0';
}

void read_input(int connectfd,int *numbytes,char *idbuffer,char *pwdbuffer){
	printf("等待输入ID\n");
	recvdata(connectfd,numbytes,idbuffer);
	printf("thread %ld recv name : %s\n",pthread_self(),idbuffer);
	//上面的sizeof问题还是没解决

	printf("等待输入密码\n");
	recvdata(connectfd,numbytes,pwdbuffer);
	printf("thread %ld recv message : %s\n",pthread_self(),pwdbuffer);
	memset(pwdbuffer,0,sizeof(pwdbuffer));

	printf("接收argv[2]\n");
	recvdata(connectfd,numbytes,pwdbuffer);
	printf("thread %ld recv what client want : %s\n",pthread_self(),pwdbuffer);
}

void process_cli(int connectfd,struct sockaddr_in client){
	char sendbuffer[MAXDATASIZE];
	char pwdbuffer[MAXDATASIZE];
	char idbuffer[MAXDATASIZE];
	int numbytes;
	
	read_input(connectfd,&numbytes,idbuffer,pwdbuffer);

	send(connectfd,"Welcome to server",18,0);

}

void *start_routine(void *arg){
	ARG *info;
	info = (ARG*)arg;
	printf("This is pthread_creat server,!!!!!start_routine here server-end!!!!!,you got a connection form %s ,and port is %d\n",inet_ntoa((info->client).sin_addr),htons((info->client).sin_port));
//	send(info->connectfd,"You got a TCP connection",25,0);

	process_cli(info->connectfd,info->client);

	pthread_exit(NULL);
}

int main()
{
	
	/*
	char *id = "my_id";
	char *pwd = "my_pwd";
	int is_register = reg(id,pwd);
	int is_login = login(id,pwd);
	int is_del = del(id,pwd);
	int is_add = add('a');
	*/

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

	err = pthread_key_create(&Key,destr);

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