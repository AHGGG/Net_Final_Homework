
# Net_Final_Homework

### 错误记录
#### Segmentation fault 2020-11-24 16:24
- main开始的时候，加上这一段代码就出错，去掉就是对的？？？？迷惑
```C
	char *id = "my_id";
	char *pwd = "my_pwd";
	int is_register = reg(id,pwd);
	int is_login = login(id,pwd);
	int is_del = del(id,pwd);
	int is_add = add('a');
```

#### field ‘client’ has incomplete type 2020-11-24 16:39
- 应该就是头文件没有引入

#### makefile make: *** [a.o] Error 1 2020-11-24 20:32
- linux下的makefile里rm *.exe了，不是在windows下，所以删去就好了
- 没有要删除的文件的时候，也会错误，空删

#### typedef的问题
```C
typedef struct {
    struct Linklist_User *next;//遍历用户list的指针，指向本类型
    char *id;
    char *pwd;
    int stat;
    char friendlist[128];
    struct Mes *p_umessage;//指向用户的消息
}Linklist_User;
```
- 这样写出错了，以后按照下面那样写
- 在节点的next域 = 指向一个node的指针的时候，出错

---

```C
typedef struct user{
    struct user *next;//遍历用户list的指针，指向本类型
    char *id;
    char *pwd;
    int stat;
    char friendlist[128];
    struct Mes *p_umessage;//指向用户的消息
}Linklist_User;
```
- 上面是没问题的，加上了struct后面的东西
- 实际上，完成了两个操作，1.定义一个新的结构类型user，2.重命名为Linklist_User

#### warning: incompatible implicit declaration of built-in function malloc’ [enabled by default] user = (Linklist_User *)malloc(sizeof(Linklist_User));
- 没有引入stdlib.h头文件

#### 逻辑错误被引发
- head和new 一个node的时候 指针域不清楚就记得初始化NULL

#### User结构体中的char friendlist[] = {0}；不行 2020-11-27
- memset()来进行这个char数组的初始化，直接={0}全初始化为0是不行的。

#### 段错误 2020-11-27
- main中的head各个字段没有初始化的时候，就出现段错误
- 有一版的register 段错误，找不到为啥
#### print_all_user_message 错误 2020-11-27
- printf的时候，不atoi，就是乱码，atoi了 就是0，不知道咋了！！

#### mysql sprintf记录
- sprintf(query_str,"select * from %s",table_name);查询表中的内容
- sprintf(query_str,"insert into user(userid,pwd,stat) value('%s','%s','%d')",id,pwd,0);//向表中插入数据

> 注意插入的收'%s', 查询的时候'%s'

- core dump：**char *query_str = NULL;**这样写就有段错误，**char query_str[100];** 这样写就好的。理解原因：因为sprintf要往里写东西，直接建立指针没有分配空间，就没法往里写，所以就core dump。 **char *query_str = NULL、malloc、memset，之后查出来是乱码说我的sql语句语法有问题？？？怎么回事，不memset查出来是空**
- core dump2:row = mysql_fetch_row(res);
> 上面少了这一行，访问row[0]也会core dump，row是 char **类型的东西

#### sprintf 记录
- sprintf(query_str, "insert into message(m_userid, m_otherid, m_message) value( '%d', '%d', '%s')", atoi(userid), atoi(otherid) , message)
> 上面的m_message收到的char * 类型的东西，这里传入的时候也要用''框住！！！


### 进度记录
- 2020-11-24 20:56---->client 根据argv[2]的值1-4判断要干什么，然后发送action，读取id发送id 读pwd，发pwd(相应server，先接受action，然后根据action判断调用哪个业务函数)
- 2020-11-24 22::20---->添加了发送信息的框架，拿到信息后怎么处理待写
- 2020-11-26--->模块的处理逻辑框架有了，但是逻辑还有问题，链表操作还有问题
- 2020-11-27--->
- 2020-11-30--->开始用mysql，放弃本地链表操作
- 2020-12-3---> 开始优化整理，添加同步消息
- 2020-12-4---> 删除无用的命令行通信函数，交作业版
