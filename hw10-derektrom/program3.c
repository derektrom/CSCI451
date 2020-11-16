/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 10 program 3 *
 * 16 November 2020         *
 * **************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>


//Main function
int main(int argc, char **argv) {
    FILE *out = fopen("output.data", "w");
    FILE *fileType1;
    FILE *fileType2;
    char buffer[100];
    int pipe2to3[2];
    //change to int 
    pipe2to3[0] = atoi(argv[1]);
    pipe2to3[1] = atoi(argv[2]);
    close(pipe2to3[1]);
    //read from pipe 2
    read(pipe2to3[0], &buffer, 100);
    //write to buffer
    fprintf(out, "%s", buffer); 
    while (read(pipe2to3[0], &buffer, 100) > 0) {
        fprintf(out, " %s", buffer);
    }
    //close pipe and file
    close(pipe2to3[0]);
    fclose(out);
    //read and print value of each type then close files
    fileType1 = fopen("shared1.dat", "r");
    fscanf(fileType1, "%s", buffer);
    printf("Type 1: %s\n", buffer);
    fclose(fileType1);
    fileType2 = fopen("shared2.dat", "r");
    fscanf(fileType2, "%s", buffer);
    printf("Type 2: %s\n", buffer);
    fclose(fileType2);
    return 0;
}
