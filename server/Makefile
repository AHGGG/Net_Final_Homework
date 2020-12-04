CC=gcc

DEL=rm

all:server

%.o: %.c %.h
	$(CC) -c *.c

#上面加-lmysqlclient不管用，要在链接的时候加才有用！！！

server:userop.o server.o 
	$(CC) *.o -o server.out -lpthread -lmysqlclient

clean:
	$(DEL) *.o *.out