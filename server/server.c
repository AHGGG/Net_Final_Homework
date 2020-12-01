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
#include "link_op.h"

#define PORT 12344
#define MAXDATASIZE 50
#define BACKLOG 5

static pthread_key_t Key;
int err;
static pthread_once_t  once = PTHREAD_ONCE_INIT;
//Linklist_User *head,*tail;//新建一个linklist_user头结点



//遍历此时注册的用户
/*void bianli_c()
{
	Linklist_User *bianl=head;
	bianl = bianl->next;
//	bianl=bianl->next;
	while(bianl != NULL)
	{
		printf("%s\n",bianl->id);
		bianl = bianl->next;
	}
}
*/
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

void read_once(int connectfd, int *numbytes, char *buffer){
	memset(buffer,0,sizeof(buffer));
	recvdata(connectfd,numbytes,buffer);
	printf("thread %ld recv message: %s\n",pthread_self(),buffer);
}

//读id--->idbuffer，读发往的id--->otheridbuffer，读信息--->message
void read_id_oid_message(int connectfd, int *numbytes, char *idbuffer, char *otheridbuffer, char *message){
	printf("等待输入ID\n");
	recvdata(connectfd,numbytes,idbuffer);
	printf("thread %ld recv name : %s\n",pthread_self(),idbuffer);

	printf("等待输入发往的id\n");
	recvdata(connectfd,numbytes,otheridbuffer);
	printf("thread %ld recv message : %s\n",pthread_self(),otheridbuffer);	

	printf("等待发送的信息\n");
	recvdata(connectfd,numbytes,message);
	printf("thread %ld recv message : %s\n",pthread_self(),message);	
}

//判断action是什么，然后调用相应的业务函数
void action(int connectfd, int *numbytes, char *idbuffer,char *pwdbuffer,char *actbuffer,char *messagebuffer){

	switch( atoi(actbuffer) ){
		case 1://register
			//read_id_pwd(connectfd,numbytes,idbuffer,pwdbuffer);
			read_once(connectfd, numbytes, idbuffer);//读id
			if( strlen(idbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去
			read_once(connectfd, numbytes, pwdbuffer);//读pwd
		        mysql_register(idbuffer,pwdbuffer);	
			//reg(head,tail,idbuffer, pwdbuffer);
			//traverselist(head,tail);
			break;
		case 2://login
			//client 两个send的东西，被一次recv了 ，这里，read_id_pwd(connectfd,numbytes,idbuffer,pwdbuffer);
			read_once(connectfd, numbytes, idbuffer);//读id
			if( strlen(idbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去
			read_once(connectfd, numbytes, pwdbuffer);//读pwd

			login(idbuffer, pwdbuffer);
			break;
		case 3://del
			read_once(connectfd, numbytes, idbuffer);//读id
			if( strlen(idbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去
			read_once(connectfd, numbytes, pwdbuffer);//读pwd

			//del(idbuffer, pwdbuffer);
			mysql_del(idbuffer, pwdbuffer);
			break;
		case 4://add
			read_once(connectfd, numbytes, idbuffer);//读id
			if( strlen(idbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

			read_once(connectfd, numbytes, pwdbuffer);//读pwd
			if( strlen(pwdbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

			read_once(connectfd, numbytes, actbuffer);//读otherid
			if( strlen(actbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

			add();
			break;
		case 5://有人要发信息
			read_once(connectfd, numbytes, idbuffer);//读id
			if( strlen(idbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

			read_once(connectfd, numbytes, pwdbuffer);//读pwd
			if( strlen(pwdbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

			read_once(connectfd, numbytes, actbuffer);//读otherid
			if( strlen(actbuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去

			read_once(connectfd, numbytes, messagebuffer);//读message
			//if( strlen(messagebuffer) > 0 ) send(connectfd,"1",2,0);//发送确认信息回去
			//read_id_oid_message(connectfd,numbytes,idbuffer,pwdbuffer,messagebuffer);//这里吧pwd当做otheridbuffer来用了
			//对上面收到的信息进行处理
			
			//sendmessage(idbuffer,pwdbuffer,messagebuffer);
			break;
		default :
			default_branch();
			break;
	}
}

//先读action，在调用action：根据action调用相应的业务函数
void read_input(int connectfd,int *numbytes, char *idbuffer, char *pwdbuffer, char *actbuffer,char *messagebuffer){
	printf("接收action-->argv[2]\n");
	recvdata(connectfd,numbytes,actbuffer);
	printf("thread %ld recv what client want : %d\n",pthread_self(), atoi(actbuffer) );

	//接收action后，判断action是什么，然后调用相应的东西进行处理
	action(connectfd,numbytes,idbuffer,pwdbuffer,actbuffer,messagebuffer);
}

void process_cli(int connectfd,struct sockaddr_in client){
	char sendbuffer[MAXDATASIZE];
	char pwdbuffer[MAXDATASIZE];
	char idbuffer[MAXDATASIZE];
	char actbuffer[MAXDATASIZE];
	char messagebuffer[MAXDATASIZE];
	int numbytes;
	
	read_input(connectfd,&numbytes,idbuffer,pwdbuffer,actbuffer,messagebuffer);
	/*1.
	//第一次调用的人，必须给TSD创建key
	pthread_once(&once,thread_init);
	
	void *data;//定义数据指针
	data = pthread_getspecific(Key);//绑定数据
	if(data == NULL){
		data = malloc(sizeof(char)*(numbytes+1));//分配数据空间
		pthread_setspecific(Key,(void*)data);//绑定TSD数据
	}
	memcpy(data,actbuffer,sizeof(char)*(numbytes+1));//memcpy 向data中拷贝数据,这里拷贝的act，返回的也是act


	send(connectfd,(char*)pthread_getspecific(Key),sizeof((char*)pthread_getspecific(Key)),0);*/
	send(connectfd, actbuffer, strlen(actbuffer), 0);

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
	//printf("query statu:%d\n",mysql_find_user_by_useid("5"));
	mysql_output_table("user");
	mysql_output_table("friend");
	mysql_output_table("message");
	
//	Linklist_User *head,*tail;
//	initdlinklist(head,tail);	   
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

	//2. err = pthread_key_create(&Key,destr);

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
