#include "userop.h"
#include "typess.h"
#include "link_op.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <mysql/mysql.h>

#define len sizeof(Linklist_User)
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

//下面这个是测试mysql连接成功与否的函数
int mysql_test( ){
    MYSQL   mysql;
    MYSQL_RES       *res = NULL;
    MYSQL_ROW       row;
    char            *query_str = NULL;
    int             rc, i, fields;

    if (NULL == mysql_init(&mysql)) {    //分配和初始化MYSQL对象
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    //尝试与运行在主机上的MySQL数据库引擎建立连接
    if (NULL == mysql_real_connect(&mysql,
                "47.106.35.199",
                "root",
                "Mysql111.",
                "netpro",
                0,
                NULL,
                0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    printf("Connected MySQL successful! \n");
 
    query_str = "select * from user";
    //char * test="test";
    //copy_string(query_str,test);
    printf("%s\n",  query_str);
    rc = mysql_real_query(&mysql, query_str, strlen(query_str));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
        return -1;
    }
    res = mysql_store_result(&mysql);
    if (NULL == res) {
         printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
         return -1;
    }
    uint64_t rows;
    rows = mysql_num_rows(res);
    printf("The total rows is: %d\n", rows);
    fields = mysql_num_fields(res);
    printf("The total fields is: %d\n", fields);
    while ((row = mysql_fetch_row(res))) {
        for (i = 0; i < fields; i++) {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }

    mysql_close(&mysql);
    return 0;
}

//根据传入的表的名字，查询出表的所有内容打印下来
int mysql_output_table( char *table_name ){
    MYSQL   mysql;
    MYSQL_RES       *res = NULL;
    MYSQL_ROW       row;

    int             rc, i, fields;

    if (NULL == mysql_init(&mysql)) {    //分配和初始化MYSQL对象
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    //尝试与运行在主机上的MySQL数据库引擎建立连接
    if (NULL == mysql_real_connect(&mysql,
                "47.106.35.199",
                "root",
                "Mysql111.",
                "netpro",
                0,
                NULL,
                0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    printf("Connected MySQL successful! \n");
 
    //query_str = "select * from user";
    char query_str[100];
    sprintf(query_str,"select * from %s",table_name);

    printf("%s\n", query_str);
    rc = mysql_real_query(&mysql, query_str, strlen(query_str));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
        return -1;
    }
    res = mysql_store_result(&mysql);
    if (NULL == res) {
         printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
         return -1;
    }
    uint64_t rows;
    rows = mysql_num_rows(res);
    printf("The total rows is: %d\n", rows);
    fields = mysql_num_fields(res);
    printf("The total fields is: %d\n", fields);
    while ((row = mysql_fetch_row(res))) {
        for (i = 0; i < fields; i++) {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }

    mysql_close(&mysql);
    return 0;
}

//根据传入的userid，查询user表，返回对应的那一行数据
//row[0]-->id row[1]-->userid row[2]-->pwd row[3]-->stat
MYSQL_ROW mysql_find_user_by_useid( char *userid ){
    MYSQL mysql;
    MYSQL_RES *res = NULL;
    MYSQL_ROW  row;
    int rc, i, fields;

    if (NULL == mysql_init(&mysql)) {    //分配和初始化MYSQL对象
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return NULL;
    }
 
    //尝试与运行在主机上的MySQL数据库引擎建立连接
    if (NULL == mysql_real_connect(&mysql,
                "47.106.35.199",
                "root",
                "Mysql111.",
                "netpro",
                0,
                NULL,
                0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return NULL;
    }
 
    printf("Connected MySQL successful! \n");

    char query_str[100];
    sprintf( query_str, "select * from user where userid = '%d'", atoi(userid) );

    printf("new_query = %s\n", query_str);
    rc = mysql_real_query(&mysql, query_str, strlen(query_str));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
        return NULL;
    }
    res = mysql_store_result(&mysql);
    if (NULL == res) {
         printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
         return NULL;
    }
    uint64_t rows;
    rows = mysql_num_rows(res);//就是有几行
    printf("The total rows is: %d\n", rows);
    fields = mysql_num_fields(res);//就是有几列
    printf("The total fields is: %d\n", fields);
    /*while ((row = mysql_fetch_row(res))) {
        for (i = 0; i < fields; i++) {
            printf("%s\t", row[i]);
        }
        printf("\n");
    }*/
    row = mysql_fetch_row(res);//少了这一行，访问row[0]也会core dump
    
    //row[0]-->id row[1]-->userid row[2]-->pwd row[3]-->stat
    if( strcmp(row[1], userid) == 0 ){
        mysql_close(&mysql);
        return row;//返回查到的那一行数据
    }
    else{
        mysql_close(&mysql);
        return NULL;
    }
}


/* 下面是mysql的业务处理函数 */
//注册函数register
int mysql_register(char *id , char *pwd){

    MYSQL           mysql;
    MYSQL_RES       *res = NULL;
    MYSQL_ROW       row;
    char            query_str[200];
    int             rc, i, fields;
    int             rows;
 
    if (mysql_init(&mysql) == NULL)      //分配和初始化MYSQL对象
    {
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    if (mysql_real_connect(&mysql,"47.106.35.199","root","Mysql111.","netpro",0,NULL,0) == NULL)
    {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    printf("Conneted MYSQL successful!\n");
    //往表中插入数据
    sprintf(query_str,"insert into user(userid,pwd,stat) value('%s','%s','%d')",id,pwd,0);
    rc = mysql_real_query(&mysql,query_str,strlen(query_str));
    if(0!= rc)
    {
	printf("mysql_real_query():%s\n",mysql_error(&mysql));
	return -1;
    }
    sprintf(query_str,"select * from user");
    rc = mysql_real_query(&mysql,query_str,strlen(query_str));
    if(0!= rc)
    {
	printf("mysql_real_query():%s\n",mysql_error(&mysql));
	return -1;
    }
    mysql_output_table("user");
    return 1;
}

int mysql_login(){}

//注销
int mysql_del(char *userid, char *pwd){
    printf("in mysql_del\n\n");
    MYSQL           mysql;
    MYSQL_RES       *res = NULL;
    MYSQL_ROW       row;
    int             rc, i, fields;
    int             rows;
 
    if (mysql_init(&mysql) == NULL)      //分配和初始化MYSQL对象
    {
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    if (mysql_real_connect(&mysql,"47.106.35.199","root","Mysql111.","netpro",0,NULL,0) == NULL)
    {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    printf("Connected MySQL successful! \n");
 
    row = mysql_find_user_by_useid(userid);
    char            query_str[200];
    if( (strcmp(row[1], userid)==0) &&  (strcmp(row[2],pwd)==0)  ){//如果找到id 且 pwd相等，那么删除id为userid的数据

        //删除id为5的数据
        sprintf(query_str, "delete from message where m_userid = '%d' or m_otherid = '%d'", atoi(userid), atoi(userid));//删除5发的，5收的信息
        rc = mysql_real_query(&mysql, query_str, strlen(query_str));
        if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return -1;
        }
    
        sprintf(query_str, "delete from friend where f_userid = '%d' or f_otherid = '%d'", atoi(userid), atoi(userid));//删除5的朋友，删除朋友是5的行
        rc = mysql_real_query(&mysql, query_str, strlen(query_str));
        if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return -1;
        }

        sprintf(query_str, "delete from user where userid = '%d'", atoi(userid));//最后删除用户id为5的行
        rc = mysql_real_query(&mysql, query_str, strlen(query_str));
        if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            
            return 1;
        }
    }else{
        return 0;//如果没有验证通过，那么就返回0
    }

    //输出看一下结果
    mysql_output_table("user");
    mysql_output_table("friend");
    mysql_output_table("message");
    
    mysql_close(&mysql);
    printf("out mysql_del\n\n");
    return 0;
}

int mysql_add(){}

int mysql_send_message(){}





//Linklist_User *head,*p1,*p2;
void visit(int c){
 	 printf("%d ",c);
}
//创建第一个节点，用作链表头部,也即双向链表初始化
int initdlinklist(Linklist_User *head, Linklist_User *tail)
{
    head=(Linklist_User *)malloc(len);
    tail=(Linklist_User *)malloc(len);
    if(!(head)||!(tail))
        return ERROR;
    /*这一步很关键*/ 
    head->pre=NULL;
    tail->next=NULL;
    /*链表为空时让头指向尾*/
    head->next=tail;
    tail->pre=head;
}
/*判定是否为空*/
int emptylinklist(Linklist_User *head,Linklist_User *tail){
    if(head->next==tail)
        return TRUE;
    else
        return FALSE;
} 
/*尾插法创建链表*/ 
int reg(Linklist_User *head,Linklist_User *tail,char *data,char *passwd){
    Linklist_User *pmove=tail,*pinsert;
    pinsert=(Linklist_User *)malloc(len);
    pinsert->id=data;
    pinsert->pwd=passwd;
    pinsert->next=NULL;
    pinsert->pre=NULL;
    tail->pre->next=pinsert;
    pinsert->pre=tail->pre;
    pinsert->next=tail;
    tail->pre=pinsert;
    return 1;
} 
/*正序打印链表*/ 
int traverselist(Linklist_User *head,Linklist_User *tail){
    Linklist_User  *pmove=head->next;
    while(pmove!=tail){
        printf("%d\t",pmove->id);
        pmove=pmove->next;
    }
    printf("\n");
}
/*
int reg(char *id, char *pwd){
	//Linklist_User *t; //用来遍历;
        Linklist_User *user = head;//用来便利linklist的
	while(user!=NULL)
	{
		if(strcmp(user->id,id)==0)
		{
			printf("it has exist！");
			break;
                }

		user = user->next;
	}	
	if(user == NULL)
	{
		int ID = atoi(id);
		if(ID<=0)
		{		
			printf("error\n");
			return 0;
		}
		else
		{
			p1 = (Linklist_User *)malloc(len);
			p1->id = id;
			p1->pwd = pwd;
			p1->message = NULL;
			p1->stat = 0;
			p1->pre = p2;
			p2 = p1;
			p2->next = NULL;
			memset( (void*)p1->friendlist, 0, sizeof(p1->friendlist));
		}
	

	}
//	bianli_c();
//	bianli_c();	
	return 1;
}
*/
/* i* 返回1，找到id pwd匹配的；返回0，登陆失败
*/
int login(char *id, char *pwd){
    printf("here is login\n");
    return 0;
}

int del(char *id, char *pwd){
    printf("here is del\n");
    return 0;
}

int add(){
    printf("here is add\n");
    return 0;
}

int sendmessage(char *send_id, char *otherid, char *message){
    printf("from id = %s to id = %s and send message = %s\n",send_id,otherid,message);
}

void default_branch(){
    printf("here is default branch\n");
}

