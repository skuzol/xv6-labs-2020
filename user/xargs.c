#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#define SPACE ('x' - 100)
#define EOL ('x' - 110)

char *my_strtok(char *s, char *delm);
int tokenize(char *s, char *delm, char **output_str);
char* my_strcat(char* destination, const char* source);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "xargs: not enough arguments\n");
    }

    //read from the pipe
    char buf_temp[128];
    char buf_final[512];

    //tokenize array from the pipe
    int ntokens = 0;
    char *left_argv[10];
    char delims[1]; delims[0] = SPACE; delims[1] = EOL;

    //read all the lines from standard input and append them to buf_final
    while (1)
    {
        gets(buf_temp, sizeof(buf_temp));
        if (buf_temp[0] == '\0')
        {
            break;
        }
        else
        {
            my_strcat(buf_final,buf_temp);
        }
    }

    //tokenize buf final using delims as delimeters 
    //and save the array to left_argv
    //return number of tokens created
    ntokens = tokenize(buf_final, delims, left_argv);

    int i, j;
    char *final_argv[20];
    //Fill array with xargs arguments
    for (i = 0; i < argc - 1; i++)
    {
        final_argv[i] = argv[i + 1];
    }
    //Fill array with left args
    for (j = 0; j < ntokens; j++)
    {
        final_argv[i + j] = left_argv[j];
    }

    //create new process and call exec
    int pid = fork();
    if (pid == 0)
    {
        exec(final_argv[0], final_argv);
        exit(3);
    }
    else if (pid > 0)
    {
        wait(0);
    }
    else
    {
        fprintf(2,"xargs: fork failed\n");
        exit(1);
    }
    
    exit(0);
}

//Returns number of tokens saved to **output_str
//*s - string to tokenize
//*delm - delimeters used for tokenization
int tokenize(char *s, char *delm, char **output_str)
{
    int ntokens = 0;
    char *p = my_strtok(s, delm);
    while (p != 0)
    {
        output_str[ntokens++] = p;
        p = my_strtok(0, delm);
    }
    return ntokens;
}

//Function found on Internet
char *my_strtok(char *str, char *comp)
{
    static int pos;
    static char *s;
    int start = pos;

    // Copying the string for further calls of strtok
    if (str != 0)
    {
        s = str;
    }

    int j = 0;
    //While not end of string
    while (s[pos] != '\0')
    {
        j = 0;
        //Comparing of one of the delimiter matches the character in the string
        while (comp[j] != '\0')
        {
            //Pos point to the next location in the string that we have to read
            if (s[pos] == comp[j])
            {
                //Replace the delimter by \0 to break the string
                s[pos] = '\0';
                pos = pos + 1;
                //Checking for the case where there is no relevant string before the delimeter.
                //start specifies the location from where we have to start reading the next character
                if (s[start] != '\0')
                    return (&s[start]);
                else
                {
                    // Move to the next string after the delimiter
                    start = pos;
                    // Decrementing as it will be incremented at the end of the while loop
                    pos--;
                    break;
                }
            }
            j++;
        }
        pos++;
    } //End of Outer while
    s[pos] = '\0';
    if (s[start] == '\0')
        return 0;
    else
        return &s[start];
}

//Function found on Internet
char* my_strcat(char* destination, const char* source)
{
    // make ptr point to the end of destination string
    char* ptr = destination + strlen(destination);
 
    // Appends characters of source to the destination string
    while (*source != '\0')
        *ptr++ = *source++;
 
    // null terminate destination string
    *ptr = '\0';
 
    // destination is returned by standard strcat()
    return destination;
}