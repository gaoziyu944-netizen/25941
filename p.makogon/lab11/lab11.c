#include <unistd.h>
#include <stdio.h>


extern char **environ; // содержит окружение текущего процесса

int execvpe(char *argv[], char *envp[]) {
    char **cur_envp = environ;
    environ = envp;

    execvp(argv[0], argv);
 
    perror("возникла ошибка execvp");
    environ = cur_envp;

    return -1;
}

// пример
int main(void) {
    char *argv[] = {"date", NULL};
    char *env[] = {"TZ=Europe/Moscow", NULL};

    if (execvpe(argv, env) == -1) return -1;
    return 0;
}

