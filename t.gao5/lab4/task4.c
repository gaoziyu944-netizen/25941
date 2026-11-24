#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1024

typedef struct Node
{
    char *str;
    struct Node *next;
} Node;

int main()
{
    char buffer[BUFFER_SIZE];
    Node *head = NULL;
    Node *tail = NULL;

    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        // 判断输入字符串的第一个字符是否为 '.'
        if (buffer[0] == '.')
        {
            break;
        }
    
        size_t len = strlen(buffer);
        // 删除末尾的换行符
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[--len] = '\0';
        }
        Node *node = malloc(sizeof(Node));
        if (node == NULL)
        {
            perror("malloc");
            break;
        }

        node->str = malloc(len + 1);
        if (node->str == NULL)
        {
            perror("malloc");
            free(node);
            break;  
        }
        memcpy(node->str, buffer, len + 1);
        node->next = NULL;
        if (head == NULL)
        {
            head = node;
        }
        else
        {
            tail->next = node;
        }
        tail = node;
    }
    // 输出链表中的所有字符串
    Node *current = head;
    while (current != NULL)
    {
        printf("%s\n", current->str);
        current = current->next;
    }
    // 释放链表占用的动态内存
    current = head;
    while (current != NULL)
    {
        Node *next = current->next;
        free(current->str);
        free(current);
        current = next;
    }
    return 0;
}