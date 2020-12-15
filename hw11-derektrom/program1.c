/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 11 program 1 *
 * 16 November 2020         *
 * **************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>


//main function
int main(int argc, char **argv) {
    FILE *inFile; 
    char buf[100];
    int pipe1to2[2];
    int semid;
    struct sembuf sem_buf;
    semid = atoi(argv[3]);
    pipe1to2[1] = atoi(argv[2]);
    inFile = fopen(argv[1], "r");
    while(fscanf(inFile, "%s", buf) != EOF) {
        sem_buf = (struct sembuf){1, -1, 0};
        semop(semid, &sem_buf, 1);
        write(pipe1to2[1], &buf, 100);
        sem_buf = (struct sembuf){0, 1, 0};
        semop(semid, &sem_buf, 1);
    }
    sem_buf = (struct sembuf){0, 1, 0};
    semop(semid, &sem_buf, 1);
    if (close(pipe1to2[1]) == -1) 
        perror("pipe_close");
    fclose(inFile);
    exit(EXIT_SUCCESS);
}
