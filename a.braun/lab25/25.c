#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd[2];
    if (pipe(fd) == -1) { 
        perror("pipe"); 
        exit(1); 
    }
    pid_t pid = fork();
    if (pid == -1) { 
        perror("fork"); 
        exit(1); 
    }
    if (pid == 0) {
        close(fd[1]);
        char buf[5012];
        ssize_t n;
        while ((n = read(fd[0], buf, sizeof buf)) > 0) 
        {
            for (ssize_t i = 0; i < n; i++)
                buf[i] = toupper((unsigned char)buf[i]);
            if (write(STDOUT_FILENO, buf, n) != n) 
            {
                perror("write"); 
                _exit(1); 
            }
        }
        if (n < 0) perror("read");
        close(fd[0]);
        _exit(0);
    }
    close(fd[0]);
    const char *text = "Hello, World!\n";
    if (write(fd[1], text, strlen(text)) == -1) perror("write");
    close(fd[1]); 
    waitpid(pid, NULL, 0);
    return 0;
}