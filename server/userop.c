#include "userop.h"
#include "typess.h"
#include <stdio.h>

int reg(char *id, char *pwd){
    User user;
    user.id = id;
    user.pwd = pwd;
    printf("here is register,and id = %s, pwd = %s\n",user.id,user.pwd);
    return 1;
}

int login(char *id, char *pwd){
    printf("here is login,and id = %s, pwd = %s\n",id,pwd);
    return 0;
}

int del(char *id, char *pwd){
    printf("here is del,and id = %s, pwd = %s\n",id,pwd);
    return 0;
}

int add(char id){
    printf("here is add\n");
    return 0;
}