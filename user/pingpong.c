#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int toParent[1];
    int toChild[1];

    if (pipe(toParent) < 0 || pipe(toChild) < 0)
    {
        printf("grind: pipe failed\n");
    }

    int pid = fork(); 
    if (pid == 0)
    {
        char bufC[1];
        read(toChild[0], &bufC, 1);
        printf("%d: received ping\n", getpid());
        write(toParent[1], &bufC, 1);
    }
    else if (pid > 0)
    {
        char bufP[1];
        write(toChild[1], "x", 1);
        read(toParent[0], &bufP, 1);
        printf("%d: received pong\n", getpid());
        wait(0);
    }
    else
    {
        fprintf(2,"grind: fork failed\n");
        exit(1);
    }

    exit(0);
}