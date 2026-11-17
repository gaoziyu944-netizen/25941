#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    FILE *file;
    printf("Before setuid():\n");
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    file = fopen("data.txt", "r");
    if (file != NULL)
    {
        printf("File opened successfully.\n");
        fclose(file);
        printf("File closed successfully.\n");
    }
    else
    {
        perror("fopen");
    }

    if (setuid(getuid()) == -1)
    {
        perror("setuid");
    }

    printf("\nAfter setuid():\n");
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    file = fopen("data.txt", "r");
    if (file != NULL)
    {
        printf("File opened successfully.\n");
        fclose(file);
        printf("File closed successfully.\n");
    }
    else
    {
        perror("fopen");
    }
    return 0;

}