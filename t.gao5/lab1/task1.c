#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <ulimit.h>
#include <limits.h>
#include <string.h>

extern char **environ;
typedef struct
{
    char option;
    char *argument;
} Option;
//输出真实用户ID、有效用户ID、真实组ID、有效组ID5 i
void option_i(void)
{
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());
    printf("Real GID: %d\n", getgid());
    printf("Effrctive GID %d\n", getegid());
}
//将当前进程设置为进程组首进程 s
void option_s(void)
{
    setpgid(0, 0);
}
//输出PID、PPID、PGID p
void option_p(void)
{
    printf("PID: %d\n", getpid());
    printf("PPID: %d\n", getppid());
    printf("PGID: %d\n", getpgrp());  
}
//输出当前ulimit资源限制值 u
void option_u(void)
{
    printf("%ld\n", ulimit(UL_GETFSIZE));
}
//修改ulimit资源限制值 Unew_ulimit
void option_U(char *argument)
{
    long value;
    value = atol(argument);
    ulimit(UL_SETFSIZE, value);
}
//输出core文件最大大小，单位为字节 c 
void option_c(void)
{
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == -1)
    {
        perror("getrlimit");
        return;
    }
    printf("%lu\n", (unsigned long)limit.rlim_max);
}
//修改core文件大小限制 Csize
void option_C(char *argument)
{
    struct rlimit limit;
    if (getrlimit(RLIMIT_CORE, &limit) == -1)
    {
        perror("getrlimit");
        return;
    }
    limit.rlim_cur = (rlim_t)atol(argument);
    if (setrlimit(RLIMIT_CORE, &limit) == -1)
    {
        perror("setrlimit");
    }


}
//输出当前工作目录 d
void option_d(void)
{
    char direstory[PATH_MAX];
    if (getcwd(direstory, sizeof(direstory)) != NULL)
    {
        printf("%s\n", direstory);
    }
}
//输出所有环境变量及其值 v
void option_v(void)
{
    char **environment = environ;
    while (*environment != NULL)
    {
        printf("%s\n", *environment);
        environment++;
    }
}
//添加或修改环境变量 Vname=value
void option_V(char *argument)
{
    char *equal_sign;
    equal_sign = strchr(argument, '=');
    if (equal_sign != NULL)
    {
        *equal_sign = '\0';
        setenv(argument, equal_sign + 1, 1);
    }
}

void process_option(Option *option)
{
    switch (option->option)
    {
        case 'i':
            option_i();
            break;

        case 's':
            option_s();
            break;

        case 'p':
            option_p();
            break;
        
        case 'u':
            option_u();
            break;
        
        case 'U':
            option_U(option->argument);
            break;
        
        case 'c':
            option_c();
            break;

        case 'C':
            option_C(option->argument);
            break;

        case 'd':
            option_d();
            break;

        case 'v':
            option_v();
            break;

        case 'V':
            option_V(option->argument);
            break;
    }
}

int main(int argc, char *argv[])
{
    int opt;
    int count = 0;
    int i;
    int capacity = 10;
    Option *options;
    options = malloc(sizeof(Option) * capacity);
    while ((opt = getopt(argc, argv, "ispU:cC:dV:uv")) != -1)
    {
        if (opt == '?')
        {
            free(options);
            return 1;
        }

        if (count >= capacity)
        {
            capacity *= 2;
            options = realloc(options, sizeof(Option) * capacity);
        }
        options[count].option = (char)opt;
        options[count].argument = optarg;
        count++;
    }
    for (i = count - 1; i >= 0; i--)
    {
        process_option(&options[i]);

    }
    free(options);
    return 0;

}