#ifndef _USEROP_H_
#define _USEROP_H_

#include "typess.h"
#include <mysql/mysql.h>

/**
 * @brief 测试mysql是否连接成功的函数
 * 
 * @return int  
 * * -1     - 连接错误
 * * 1      - 连接成功
 */
int mysql_test( );

/**
 * @brief 打印一个表中的数据
 * 
 * @param table_name 要打印的表名
 * @return int 
 * * -1     - 连接错误 || 查询错误 || 无结果
 * * 1      - 打印成功
 */
int mysql_output_table( char *table_name );

/**
 * @brief 根据userid查询user表
 * 
 * @param userid 查询用户的id
 * @return MYSQL_ROW
 * * NULL       - 查询失败 || 连接错误
 * * MYSQL_ROW      - 查询成功, 返回userid对应的那一行数据
 * * NULL       - 没有查询到(查询结果为0行)
 */
MYSQL_ROW mysql_find_user_by_useid( char *userid );

/**
 * @brief 根据输入的id、pwd在mysql中注册用户
 * 
 * @param userid 注册用户id
 * @param pwd 注册用户密码
 * @return int 
 * * -1     - 连接错误 || 插入失败
 * * 1      - 注册成功
 */
int mysql_register(char *userid , char *pwd);

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
int mysql_login(char *userid , char *pwd);

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
int mysql_del(char *userid, char *pwd);

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
int mysql_add(char *userid, char *pwd, char *otherid);

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
int mysql_send_message(char *userid, char *pwd, char *otherid, char *message);

/**
 * @brief 不知道client要干什么，在逻辑之外
 * 
 */
void default_branch();

#endif
