#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>

static volatile sig_atomic_t sigint_count = 0;
static struct termios old_t;

static void handler(int sig) 
{
    if (sig == SIGINT) 
    {
        putchar('\a');          
        fflush(stdout);
        sigint_count++;
    } 
    else if (sig == SIGQUIT) 
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
        printf("\nSIGINT received %d times\n", (int)sigint_count);
        exit(EXIT_SUCCESS);
    }
}

int main() 
{
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;           
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    tcgetattr(STDIN_FILENO, &old_t);
    struct termios t = old_t;
    t.c_lflag &= ~ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    for (;;) pause();                
    return 0;
}