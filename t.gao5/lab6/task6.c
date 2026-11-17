#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define MAX_LINES 1000

struct Line {
    off_t offset;
    int length;
};

int fd;

// 超时处理函数
void handler(int sig)
{
    char buffer[4096];
    ssize_t n;

    lseek(fd, 0, SEEK_SET);

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, n);
    }

    _exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct Line lines[MAX_LINES];
    int count = 0;
    off_t start = 0;
    char c;

    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            off_t pos = lseek(fd, 0L, SEEK_CUR);
            if (pos == (off_t)-1) {
                perror("lseek");
                close(fd);
                return 1;
            }
            if (count >= MAX_LINES) {
                fprintf(stderr, "Too many lines (max %d)\n", MAX_LINES);
                close(fd);
                return 1;
            }
            // 记录当前行的起始偏移量
            lines[count].offset = start;
            // 计算当前行的长度
            lines[count].length = (int)(pos - start);
            // 更新下一行的起始偏移量
            start = pos;
            // 行数加 1
            count++;
        }
    }

    off_t end = lseek(fd, 0L, SEEK_CUR);
    if (start < end) {
        if (count >= MAX_LINES) {
            fprintf(stderr, "Too many lines (max %d)\n", MAX_LINES);
            close(fd);
            return 1;
        }
        lines[count].offset = start;
        lines[count].length = (int)(end - start);
        count++;
    }

    printf("Line    Offset  Length\n");
    for (int i = 0; i < count; i++)
        printf("%-7d %-7ld %d\n", i + 1, (long)lines[i].offset, lines[i].length);
    printf("\n");

    // 注册 SIGALRM 信号处理函数
    signal(SIGALRM, handler);

    int number;
    while (1) {
        printf("Enter line number (0 to exit): ");
        fflush(stdout);

        // 设置 5 秒定时器
        alarm(5);

        if (scanf("%d", &number) != 1 || number == 0) {
            alarm(0);
            break;
        }

        // 取消定时器
        alarm(0);

        if (number < 1 || number > count) {
            printf("Invalid line number\n");
            continue;
        }

        char buffer[4096];
        int length = lines[number - 1].length;

        if (length >= (int)sizeof(buffer)) {
            printf("Line is too long\n");
            continue;
        }

        if (lseek(fd, lines[number - 1].offset, SEEK_SET) == (off_t)-1) {
            perror("lseek");
            break;
        }

        ssize_t r = read(fd, buffer, length);
        if (r != length) {
            perror("read");
            break;
        }
        buffer[length] = '\0';

        printf("%s", buffer);
        if (length == 0 || buffer[length - 1] != '\n')
            printf("\n");
    }

    close(fd);
    return 0;
}
