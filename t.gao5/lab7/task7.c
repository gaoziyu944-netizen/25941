
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <setjmp.h>

#define MAX_LINES 1000

struct Line {
    off_t offset;
    int length;
};

char *data;
size_t file_size;
jmp_buf env;

// 超时处理函数
void handler(int sig)
{
    longjmp(env, 1);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    file_size = (size_t)st.st_size;

    // 将文件映射到内存
    if (file_size > 0) {
        // 让操作系统选择映射地址,映射文件的字节数,只允许读取,私有映射,要映射的文件描述符,从文件开头开始映射
        data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

        if (data == MAP_FAILED) {
            perror("mmap");
            close(fd);
            return 1;
        }
    }

    close(fd);

    struct Line lines[MAX_LINES];
    int count = 0;
    size_t start = 0;

    // 建立行号、偏移量和长度表
    for (size_t i = 0; i < file_size; i++) {
        if (data[i] == '\n') {
            if (count >= MAX_LINES) {
                fprintf(stderr, "Too many lines (max %d)\n", MAX_LINES);
                if (file_size > 0)
                    munmap(data, file_size);
                return 1;
            }
            // 保存当前行的起始偏移量
            lines[count].offset = (off_t)start;
            // 保存当前行的长度
            lines[count].length = (int)(i + 1 - start);
            // 更新下一行的起始位置
            start = i + 1;
            // 已经成功记录一行，行数加 1
            count++;
        }
    }

    // 处理最后一行没有换行符的情况
    if (start < file_size) {
        if (count >= MAX_LINES) {
            fprintf(stderr, "Too many lines (max %d)\n", MAX_LINES);
            if (file_size > 0)
                munmap(data, file_size);
            return 1;
        }

        lines[count].offset = (off_t)start;
        lines[count].length = (int)(file_size - start);
        count++;
    }

    printf("Line    Offset  Length\n");

    for (int i = 0; i < count; i++) {
        printf("%-7d %-7ld %d\n",
               i + 1,
               (long)lines[i].offset,
               lines[i].length);
    }

    printf("\n");

    // 注册 SIGALRM 信号处理函数
    signal(SIGALRM, handler);

    int number;

    while (1) {
        printf("Enter line number (0 to exit): ");
        fflush(stdout);

        // 设置 5 秒定时器
        if (setjmp(env) != 0) {
            alarm(0);

            printf("\nTime is up. Printing the whole file:\n");

            if (file_size > 0)
                // 数据起始地址,每个数据单位 1 字节,输出多少个字节,输出到标准输出
                fwrite(data, 1, file_size, stdout);

            printf("\n");

            break;
        }

        alarm(5);

        if (scanf("%d", &number) != 1 || number == 0) {
            alarm(0);
            break;
        }

        // 用户成功输入，取消定时器
        alarm(0);

        if (number < 1 || number > count) {
            printf("Invalid line number\n");
            continue;
        }

        // 从内存映射中读取指定行
        printf("%.*s",
               lines[number - 1].length,        // 指定输出的最大长度
               data + lines[number - 1].offset);// 指定行的起始地址
        // 检查当前行是否需要额外输出换行符
        if (lines[number - 1].length == 0 ||
            data[lines[number - 1].offset +
                 lines[number - 1].length - 1] != '\n') {
            printf("\n");
        }
    }

    // 解除内存映射
    if (file_size > 0)
        munmap(data, file_size);

    return 0;
}
