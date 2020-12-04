#include "userop.h"
#include "typess.h"

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <mysql/mysql.h>

#define SERVERADDR "47.106.35.199"
#define MYSQLUSER "root"
#define MYSQLPASSWORD "Mysql111."
#define DATABASENAME "netpro"

/**
 * @brief 测试mysql是否连接成功的函数
 * 
 * @return int  
 * * -1     - 连接错误
 * * 1      - 连接成功
 */
int mysql_test( ){
    MYSQL   mysql;

    if (NULL == mysql_init(&mysql)) {    //分配和初始化MYSQL对象
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    //尝试与运行在主机上的MySQL数据库引擎建立连接
    if (NULL == mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    printf("Connected MySQL successful! \n");
    mysql_close(&mysql);
    return 1;
}

/**
 * @brief 打印一个表中的数据
 * 
 * @param table_name 要打印的表名
 * @return int 
 * * -1     - 连接错误 || 查询错误 || 无结果
 * * 1      - 打印成功
 */
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
    if (NULL == mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    printf("Connected MySQL successful! \n");
 
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
    return 1;
}

/**
 * @brief 根据userid查询user表
 * 
 * @param userid 查询用户的id
 * @return MYSQL_ROW
 * * NULL       - 查询失败 || 连接错误
 * * MYSQL_ROW      - 查询成功, 返回userid对应的那一行数据
 * * NULL       - 没有查询到(查询结果为0行)
 */
MYSQL_ROW mysql_find_user_by_useid( char *userid ){
    printf("in mysql_find_user_by_userid\n\n");
    MYSQL mysql;
    MYSQL_RES *res = NULL;
    MYSQL_ROW  row;
    int rc, i, fields;

    if (NULL == mysql_init(&mysql)) {    //分配和初始化MYSQL对象
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return NULL;
    }
 
    //尝试与运行在主机上的MySQL数据库引擎建立连接
    if ( NULL == mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) ) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return NULL;
    }
 
    //printf("Connected MySQL successful! \n");

    char query_str[100];
    sprintf( query_str, "select * from user where userid = '%d'", atoi(userid) );

    printf("mysql_find_user_by_useid = %s\n\n", query_str);
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
    rows = mysql_num_rows(res);
    fields = mysql_num_fields(res);
    printf("The total rows is: %d , the total fields is: %d\n", rows,fields);
    if( rows == 0 ) {
        mysql_close(&mysql);
        return NULL;
    }

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

int mysql_find_message_by_useid( char *userid ,char *returnbuffer){
    printf("in mysql_find_message_by_userid\n\n");
    MYSQL mysql;
    MYSQL_RES *res = NULL;
    MYSQL_ROW  row;
    int rc, i, fields;

    if (NULL == mysql_init(&mysql)) {    //分配和初始化MYSQL对象
        printf("mysql_init(): %s\n", mysql_error(&mysql));
        return 0;
    }
 
    //尝试与运行在主机上的MySQL数据库引擎建立连接
    if ( NULL == mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) ) {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return 0;
    }
 
    char query_str[100];
    sprintf( query_str, "select * from message where m_otherid = '%d'", atoi(userid) );//otherid，因为是检索有没有发给自己的

    printf("mysql_find_message_by_useid = %s\n\n", query_str);
    rc = mysql_real_query(&mysql, query_str, strlen(query_str));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(&mysql));
        return 0;
    }
    res = mysql_store_result(&mysql);
    if (NULL == res) {
         printf("mysql_restore_result(): %s\n", mysql_error(&mysql));
         return 0;
    }
    uint64_t rows;
    rows = mysql_num_rows(res);
    fields = mysql_num_fields(res);
    printf("The total rows is: %d , the total fields is: %d\n", rows,fields);
    if( rows == 0 ) {
        mysql_close(&mysql);
        return 0;
    }
    //row[0]-->m_id row[1]-->m_userid row[2]-->m_otherid row[3]-->m_message
    char onemessage[4096] = {0};
    while (  ( row = mysql_fetch_row(res) )  ) {
                sprintf(returnbuffer, "from %s to %s : %s next message-->", row[1], row[2], row[3]);//每次取得一行数据都进行拼接
                strcat(onemessage,returnbuffer);//将这次returnbuffer中的东西存进onemessage
                printf("返回的消息 = %s\n",onemessage);
    }
    sprintf(returnbuffer,onemessage);
    mysql_close(&mysql);
    return 1;//返回查到的那一行数据
    
    /*if( strcmp(row[2], userid) == 0 ){    } else{
        mysql_close(&mysql);
        return NULL; }*/
}

/**
 * @brief 写user表中的stat为0或1
 * 
 * @param userid  用户id
 * @param pwd  用户密码
 * @param stat 0:离线 1:在线
 * @return int 
 * * -1     - 连接错误 || 查询失败 || 写入失败
 * * 1      - 退出成功
 * * 0      - 密码错误
 */
int mysql_set_stat(char *userid, char *pwd, int stat){
    printf("in mysql_quit\n\n");
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
 
    if ( mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) == NULL)
    {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    row = mysql_find_user_by_useid(userid);

    if( row == NULL) {
        printf("userid = %s 不存在与user表中, 添加失败!!\n", userid);
        mysql_close(&mysql);
        return 0;
    }

    char            query_str[200];
    //走到这里，userid 有对应的人的，那么就验证id pwd。如果id pwd验证通过--->写stat
    if( (strcmp(row[1], userid)==0) &&  (strcmp(row[2],pwd)==0) ) {//如果找到id 且 pwd相等，就写user->stat = 0
        //写user表中的stat
        sprintf(query_str, "update user set stat = '%d' where userid = '%d'", stat, atoi(userid) );//写stat
        rc = mysql_real_query(&mysql, query_str, strlen(query_str));
        if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return -1;
        }
        mysql_close(&mysql);
        printf("退出成功\n");
        mysql_output_table("user");
        return 1;
    }else{
        mysql_close(&mysql);
        return 0;//如果id pwd没有验证通过，那么就返回0
    }

}

/**
 * @brief 根据输入的id、pwd在mysql中注册用户
 * 
 * @param userid 注册用户id
 * @param pwd 注册用户密码
 * @return int 
 * * -1     - 连接错误 || 插入失败
 * * 1      - 注册成功
 */
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
 
    if (mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) == NULL)
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

/**
 * @brief 根据输入的id、pwd在mysql中登录用户
 * 
 * @param userid 登录用户id
 * @param pwd 登录用户密码
 * @return int 
 * * -1     - 连接错误 || 登录失败
 * * 0      - 查询结果为0
 * * 1      - 登录成功
 */
int mysql_login(char *userid , char *pwd){
	
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
 
    if ( mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) == NULL)
    {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    printf("Conneted MYSQL successful!\n");
    row = mysql_find_user_by_useid(userid );
    if( row == NULL ){
    	return 0;
    }else{
        if( strcmp(row[2],pwd) == 0 ){
            printf("login is successful!\n");
            mysql_output_table("user");
            printf("登录成功\n");

            mysql_set_stat(userid,pwd,1);
            return 1;
        }else{
		printf("error!\n");
		return -1;
	}
    }

}

/**
 * @brief 验证通过后，根据输入的id、pwd在friend、message表中删除有关表项，最后删除user表中的userid对应表项
 * 
 * @param userid 注销用户id
 * @param pwd 注销用户密码
 * @return int 
 * * -1     - 连接错误 || 查询失败 || 删除失败
 * * 1      - 注销成功
 * * 0      - 密码错误
 */
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
 
    if ( mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) == NULL)
    {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    //printf("Connected MySQL successful! \n");
 
    row = mysql_find_user_by_useid(userid);
    char            query_str[200];
    if( row == NULL ){
        //说明没有在user表中找到userid对应的人，就直接返回
        printf("没有找到要del的人\n");
        mysql_close(&mysql);
        return 0;//如果没有验证通过，那么就返回0
    }
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
            return -1;
        }

        mysql_close(&mysql);
        printf("删除成功!\n");
        return 1;
    }else{
        printf("id pwd不匹配\n");
        mysql_close(&mysql);
        return 0;//如果没有验证通过，那么就返回0
    }
}

/**
 * @brief 验证通过后，根据输入的id、pwd在friend表中添加相应表项
 * 
 * @param userid 用户id
 * @param pwd 用户密码
 * @param otherid 想要加的好友的id
 * @return int 
 * * -1     - 连接错误 || 查询失败 || 写入失败
 * * 1      - 添加成功
 * * 0      - 密码错误
 */
int mysql_add(char *userid, char *pwd, char *otherid){
    printf("in mysql_add\n\n");
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
 
    if ( mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) == NULL)
    {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
    //printf("Connected MySQL successful! \n");
 
    row = mysql_find_user_by_useid(userid);
    MYSQL_ROW rowother = mysql_find_user_by_useid(otherid);

    if( rowother == NULL || row == NULL) {
        printf("userid = %s | otherid = %s 有一个不存在与user表中, 添加失败!!\n",userid, otherid);
        mysql_close(&mysql);
        return 0;
    }

    char            query_str[200];
    //走到这里，userid otherid都是有对应的人的，那么就验证id pwd。如果id pwd验证通过--->能加好友，否则不能
    if( (strcmp(row[1], userid)==0) &&  (strcmp(row[2],pwd)==0) ) {//如果找到id 且 pwd相等，就写friend表
        //在friend表中添加表项
        sprintf(query_str, "insert into friend(f_userid,f_otherid) value( '%d', '%d')", atoi(userid), atoi(otherid) );//添加好友
        rc = mysql_real_query(&mysql, query_str, strlen(query_str));
        if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return -1;
        }
        mysql_close(&mysql);
        printf("添加好友成功\n");
        mysql_output_table("friend");
        return 1;
    }else{
        mysql_close(&mysql);
        return 0;//如果id pwd没有验证通过，那么就返回0
    }
}

/**
 * @brief 来自userid的人，想给otherid的人，发送message
 * 
 * @param userid 发消息人的id
 * @param pwd 发消息人的密码
 * @param otherid 想发给谁
 * @param message 想发的消息
 * @return int 
 * * -1     - 连接错误 || 查询失败 || 写入失败
 * * 1      - 写入消息成功
 * * 0      - 密码错误
 */
int mysql_send_message(char *userid, char *pwd, char *otherid, char *message){

    printf("in mysql_send_message\n\n");
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
 
    if ( mysql_real_connect(&mysql,SERVERADDR, MYSQLUSER, MYSQLPASSWORD, DATABASENAME,0,NULL,0) == NULL)
    {
        printf("mysql_real_connect(): %s\n", mysql_error(&mysql));
        return -1;
    }
 
    row = mysql_find_user_by_useid(userid);//看userid对应的人在不在
    MYSQL_ROW rowother = mysql_find_user_by_useid(otherid);//看otherid对应的人在不在

    if( rowother == NULL || row == NULL) {
        printf("userid = %s | otherid = %s 有一个不存在与user表中, 添加失败!!\n",userid, otherid);
        mysql_close(&mysql);
        return 0;
    }

    char            query_str[200];
    //走到这里，userid otherid都是有对应的人的，那么就验证id pwd。如果id pwd验证通过--->能【发送消息】，否则不能
    if( (strcmp(row[1], userid)==0) &&  (strcmp(row[2],pwd)==0) ) {//如果找到id 且 pwd相等，就写friend表
        //在message表中添加表项
        sprintf(query_str, "insert into message(m_userid, m_otherid, m_message) value( '%d', '%d', '%s')", atoi(userid), atoi(otherid) , message);
        rc = mysql_real_query(&mysql, query_str, strlen(query_str));
        if (0 != rc) {
            printf("mysql_real_query(): %s\n", mysql_error(&mysql));
            return -1;
        }

        mysql_close(&mysql);
        printf("写入消息成功\n");
        mysql_output_table("message");
        return 1;

    }else{
        printf("发送消息的人密码错误!!!!!!!\n");
        mysql_close(&mysql);
        return 0;//如果id pwd没有验证通过，那么就返回0
    }

}

/**
 * @brief 不知道client要干什么，在逻辑之外
 * 
 */
void default_branch(){
    printf("Here is default branch\n");
}
