#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

void write_file(int fd, const char *infile);
void doWrite(int fd, const char *buff, ssize_t len);

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4)
    {
        printf("Usage: ./fconc infile1 infile2 [outfile (default:fconc.out)] \n");  // Wrong number of arguments
        exit(1);    // exit program
    }

    int fd, osFlags, filePerms;

    osFlags = O_CREAT | O_WRONLY | O_TRUNC;    // flags to create a file  
                                               
    filePerms = S_IRUSR | S_IWUSR;             // permission read-write to owner of the file

    if (argv[3] == NULL) argv[3] = "fconc.out";    // 2 arguments case, output file is created with the name "fconc.out" 
    fd = open(argv[3], osFlags, filePerms);        // open output file with descriptor "fd" with flags and permissions 
    if (fd < 0)
    {
        perror("Open");    // error message if open output file has been erroneous
        exit(1);
    }

    write_file(fd, argv[1]);    // argv[1]: the 1st argument, input file
    write_file(fd, argv[2]);    // argv[2]: the 2nd argument, input file    

    if (close(fd) < 0)
    {
        perror("Close");    // error message if close output file has been erroneous
        exit(2);
    }
    return 0;
}

void write_file(int fd, const char *infile)
{
    int fd1;    
    char buff[BUFF_SIZE];    // a buffer
    ssize_t rcnt;            // buffer block size

    fd1 = open(infile, O_RDONLY);    // open input file  
    if (fd1 < 0)
    {
        perror(infile);  // error message if open input file has been erroneous
        exit(1);
    }    

    while (((rcnt = read(fd1, buff, BUFF_SIZE)) != 0))    // while fd hasn't found end of file
    {
        if (rcnt == -1)
        {
            perror("Read");
            exit(1);
        }    
        doWrite(fd, buff, rcnt);    // write buffer to output file
    }

    if (close(fd1) < 0)
    {
        perror("Close");     // error message if close input file has been erroneous 
        exit(-1);
    }
}

void doWrite(int fd, const char *buff, ssize_t len)
{
    if (write(fd, buff, len) != len)     // write buffer to output file and check if all bytes have been written
    {
        perror("Couldn't write whole buffer!");    
        exit(1);
    }
}
