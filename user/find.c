#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

//Function definitions
void print_fileinfo(struct stat st);           //debugging help
char *append_str(char *dest, const char *src); //Stackoverflow code
void find_file(int fd, char *path, char *filename);

int main(int argc, char *argv[])
{
    if ((argc < 3) || (argc > 3))
    {
        fprintf(2,"Too few/much arguments to function \"find\"\n");
        fprintf(2,"Usage: find {path} {filename}\n");
        exit(0);
    }

    int fd;
    struct stat st;

    if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open \"%s\"\n", argv[1]);
        exit(0);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat \"%s\"\n", argv[1]);
        close(fd);
        exit(0);
    }

    if (st.type != T_DIR)
    {
        fprintf(2, "find: \"%s\" not a folder\n", argv[1]);
        close(fd);
        exit(0);
    }

    find_file(fd, argv[1], argv[2]);

    exit(0);
}

void find_file(int fd, char *path, char *filename)
{
    struct dirent de;

    while (read(fd, &de, sizeof(de)) != 0)
    {
        if ((strcmp(de.name, ".") == 0) || (strcmp(de.name, "..") == 0) || (de.name[0] == '\0'))
        {
            continue;
        }

        char next_dir_path[512];
        strcpy(next_dir_path, path);
        append_str(next_dir_path, "/");
        append_str(next_dir_path, de.name);

        int next_dir_fd;
        if ((next_dir_fd = open(next_dir_path, O_RDONLY)) < 0)
        {
            fprintf(2, "find: cannot open \"%s\"\n", next_dir_path);
            exit(0);
        }

        struct stat st;
        if (fstat(next_dir_fd, &st) < 0)
        {
            fprintf(2, "find: cannot stat \"%s\"\n", path);
            close(fd);
            exit(0);
        }

        if ((st.type == T_FILE) && (strcmp(de.name, filename) == 0))
        {
            printf("%s/%s\n", path, de.name);
        }

        if (st.type == T_DIR)
        {
            find_file(next_dir_fd, next_dir_path, filename);
        }
        close(next_dir_fd);
    }
}

void print_fileinfo(struct stat st)
{
    printf("FileInfo:\n");
    printf("- dev: %d\n", st.dev);
    printf("- ino: %d\n", st.ino);
    printf("- type: %d\n", st.type);
    printf("- nlink: %d\n", st.nlink);
    printf("- size: %d\n", st.size);
}

char *append_str(char *dest, const char *src)
{
    int i, j;
    for (i = 0; dest[i] != '\0'; i++)
        ;
    for (j = 0; src[j] != '\0'; j++)
        dest[i + j] = src[j];
    dest[i + j] = '\0';
    return dest;
}

//sh < xargstest.sh