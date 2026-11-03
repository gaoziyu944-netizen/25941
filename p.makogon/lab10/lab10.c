#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("не указано имя команды: ожидалось - имя команды и аргументы для нее");
        return 1;
    }

    pid_t pid = fork();
    
    if (pid == -1) {
        perror("не удалось создать дочерний процесс");
        return 1;
    }
    else if (pid == 0) {
        // дочерний процесс
        execvp(argv[1], &argv[1]);

        perror("возникла ошибка выполнения команды");
        _exit(1);
    }
    else {
        // родитель
        int status;

        if (waitpid(pid, &status, 0) == -1) {
            perror("возникла ошибка waitpid");
            return 1;
        }

        if (WIFEXITED(status)) {
            printf("команда завершилась с кодом: %d\n", WEXITSTATUS(status));
            return WEXITSTATUS(status);
        }
        else if (WIFSIGNALED(status)) {
            printf("команда завершилась сигналом: %d\n", WTERMSIG(status));
            return 128 + WTERMSIG(status);
        }
        else {
            printf("команда как то завершилась\n");
            return 1;
        }
    }

    return 0;
}