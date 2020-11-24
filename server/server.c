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
#define MAXDATASIZE 50
#define BACKLOG 5

static pthread_key_t Key;
int err;
static pthread_once_t  once = PTHREAD_ONCE_INIT;

void destr(void *arg){
	printf("destroy memory, pthread_self is %ld\n\n",pthread_self());
	free(arg);
}

void thread_init(){
	pthread_key_create(&Key,destr);
}

void recvdata(int connectfd, int *numbytes, char *buffer){
	if( (*numbytes = recv(connectfd,buffer,MAXDATASIZE,0)) == -1  ){
		perror("recv error.");
		exit(1);
	}
	buffer[*numbytes] = '\0';
}

void read_id_pwd(int connectfd, int *numbytes, char *idbuffer, char *pwdbuffer){
	printf("等待输入ID\n");
	recvdata(connectfd,numbytes,idbuffer);
	printf("thread %ld recv name : %s\n",pthread_self(),idbuffer);

	printf("等待输入密码\n");
	recvdata(connectfd,numbytes,pwdbuffer);
	printf("thread %ld recv message : %s\n",pthread_self(),pwdbuffer);
}

//判断action是什么，然后调用相应的业务函数
void action(int connectfd, int *numbytes, char *idbuffer,char *pwdbuffer,char *actbuffer){

	switch( atoi(actbuffer) ){
		case 1://register
			read_id_pwd(connectfd,numbytes,idbuffer,pwdbuffer);
			reg(idbuffer, pwdbuffer);
			break;
		case 2://login
			read_id_pwd(connectfd,numbytes,idbuffer,pwdbuffer);
			login(idbuffer, pwdbuffer);
			break;
		case 3://del
			read_id_pwd(connectfd,numbytes,idbuffer,pwdbuffer);
			del(idbuffer, pwdbuffer);
			break;
		case 4://add
			add();
			break;
		default :
			default_branch();
			break;
	}

}

//先读action，在调用action：根据action调用相应的业务函数
void read_input(int connectfd,int *numbytes, char *idbuffer, char *pwdbuffer, char *actbuffer){
	printf("接收action-->argv[2]\n");
	recvdata(connectfd,numbytes,actbuffer);
	printf("thread %ld recv what client want : %d\n",pthread_self(), atoi(actbuffer) );

	//接收action后，判断action是什么，然后调用相应的东西进行处理
	action(connectfd,numbytes,idbuffer,pwdbuffer,actbuffer);
}

void process_cli(int connectfd,struct sockaddr_in client){
	char sendbuffer[MAXDATASIZE];
	char pwdbuffer[MAXDATASIZE];
	char idbuffer[MAXDATASIZE];
	char actbuffer[MAXDATASIZE];
	int numbytes;
	
	read_input(connectfd,&numbytes,idbuffer,pwdbuffer,actbuffer);
	//第一次调用的人，必须给TSD创建key
	pthread_once(&once,thread_init);
	
	void *data;//定义数据指针
	data = pthread_getspecific(Key);//绑定数据
	if(data == NULL){
		data = malloc(sizeof(char)*(numbytes+1));//分配数据空间
		pthread_setspecific(Key,(void*)data);//绑定TSD数据
	}
	memcpy(data,actbuffer,sizeof(char)*(numbytes+1));//memcpy 向data中拷贝数据,这里拷贝的act，返回的也是act


	send(connectfd,(char*)pthread_getspecific(Key),sizeof((char*)pthread_getspecific(Key)),0);

}

void *start_routine(void *arg){
	ARG *info;
	info = (ARG*)arg;
	printf("This is pthread_creat server,!!!!!start_routine here server-end!!!!!,you got a connection form %s ,and port is %d\n",inet_ntoa((info->client).sin_addr),htons((info->client).sin_port));
	process_cli(info->connectfd,info->client);
	pthread_exit(NULL);
}

int main()
{
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
