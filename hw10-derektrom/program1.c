/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 10 program 1 *
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
    FILE *inFile = fopen("input.data", "r");
    char buffer[100];
    int pipe1to2[2];
    int semaphoreID;
    struct sembuf unlock;
    struct sembuf lock;
    //Set-up samphore locks and unlocks
    unlock.sem_num = 0;
    unlock.sem_op = 1;
    unlock.sem_flg = 0;
    lock.sem_num = 0;
    lock.sem_op = -1;
    lock.sem_flg = 0;
    //Convert pipe and sem ID's to int's
    semaphoreID = atoi(argv[3]);
    pipe1to2[0] = atoi(argv[1]);
    pipe1to2[1] = atoi(argv[2]);
    //Make the pipe non-blocking on write
    fcntl(pipe1to2[1], F_SETFL, O_NONBLOCK);
    close(pipe1to2[0]);
    fscanf(inFile, "%s", buffer);
    while(1) {
        if (semop(semaphoreID, &lock, 1) == -1) {
            perror("semop-mutex_lock");
            exit(EXIT_FAILURE);
        }
        //If the pipe is full then release 
        if (write(pipe1to2[1], &buffer, 100) < 0) {
            semop(semaphoreID, &unlock, 1);
            continue;
        }
        //read next 
        semop(semaphoreID, &unlock, 1);
        if (fscanf(inFile, "%s", buffer) == EOF) {
            break;
        }
    }
    if (close(pipe1to2[1]) == -1) {
        perror("Unable to close pipe\n"); 
    }
    fclose(inFile);
    exit(EXIT_SUCCESS);
}
