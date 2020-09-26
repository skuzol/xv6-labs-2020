#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

//Function definitions
void child_func(int fd_read);

int main(int argc, char *argv[])
{
    //create pipe
    int fd[1];
    if (pipe(fd) < 0)
        printf("grind: pipe failed\n");

    int pid = fork();
    //parent process
    if (pid > 0)
    {
        close(fd[0]);
        for (int i = 2; i <= 35; i++)
        {
            write(fd[1], &i, 1);
        }
        close(fd[1]);
        wait(0);
    }
    //child process
    else if (pid == 0)
    {
        close(fd[1]);
        child_func(fd[0]);
        close(fd[0]);
    }
    exit(0);
}

void child_func(int fd_read)
{
    int p, n, isFirst = 1;
    //p = get a number from left neighbor
    //print p
    read(fd_read, &p, 1);
    printf("prime %d\n", p);

    int new_fd[1];
    if (pipe(new_fd) < 0)
        printf("grind: pipe failed\n");

    //loop:
    //n = get a number from left neighbor
    while (read(fd_read, &n, 1) != 0)
    {
        //if (p does not divide n)
        //  send n to right neighbor
        if (n % p != 0)
        {
            //First n creates a new process(right neighbor).
            if (isFirst)
            {
                isFirst = 0;
                int new_pid = fork();
                if (new_pid == 0)
                {
                    close(new_fd[1]);
                    child_func(new_fd[0]);
                    close(new_fd[0]);
                    break;
                }
                else if (new_pid > 0)
                {
                    close(new_fd[0]);
                }
            }
            //Sending n to right neighbor.
            write(new_fd[1], &n, 1);
        }
    }
    //Pipe is empty, wait for child to finish.
    close(fd_read);
    close(new_fd[1]);
    wait(0);
}