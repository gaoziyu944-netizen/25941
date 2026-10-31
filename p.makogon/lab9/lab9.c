#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("не указано имя файла");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("не удалось создать дочерний процесс");
        return 1;
    }
    else if (pid == 0) {
        //  дочерний процесс
        execlp("cat", "cat", argv[1], NULL); // вместо ребенка cat
        perror("возникла ошибка execlp");
        _exit(1);
    }
    else {
        // родитель
        printf("==================== РОДИТЕЛЬ ЖДЕТ ====================\n");

        fflush(stdout);

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("возникла ошибка waitpid");
            return 1;
        }

        printf("==================== ЭТО ВЫВЕЛОСЬ ПОСЛЕ CAT ====================\n");

        fflush(stdout);

    }

    return 0;
}