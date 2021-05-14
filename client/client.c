#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

#include "clientop.h"

#define PORT 		12344
#define MAXDATASIZE	100
//
int main(int argc, char *argv[])
{	
	int fd, numbytes,connResult;
	char buf[MAXDATASIZE];
	struct hostent *he;
	struct sockaddr_in server, reply;
	if (argc != 3)
	{
		printf("Usage: %s  <IP addrress> <what you want: 1.register/ 2.login/ 3.del/ 4.add>\n", argv[0]);
		exit(-1);
	}
	if ((he = gethostbyname(argv[1])) == NULL) {
		printf("gethostbyname error.");
		exit(1);
	}
	if ((fd = socket(AF_INET, SOCK_STREAM,0)) == -1) {
		printf("Create socket failed.");
		exit(1);
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr *) he->h_addr);
	socklen_t len = sizeof(server);

	if ( (connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr))) == -1) {
		perror("connect failed.");
		exit(1);
	}

	switch( atoi(argv[2]) ){
		case 1://register
			do_register(fd,buf);
			break;
		case 2://login
			do_login(fd,buf);
			break;
		case 3://del
			do_del(fd,buf);
			break;
		case 4://add
			do_add(fd,buf);
			break;
		case 5://send message
			sendmessage(fd,buf);
			break;		
		default :
			do_default(fd,buf);
			break;
	}

	if( (numbytes = recv(fd, buf, MAXDATASIZE, 0)) == -1){
		perror("recv error.");
		exit(1);
	}

	buf[numbytes] = '\0';
	printf("Recv reversed message from server: %s\n",buf);
	
	close(fd);
	return 0;
}

