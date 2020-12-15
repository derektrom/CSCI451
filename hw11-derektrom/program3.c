/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 11 program 3 *
 * 16 November 2020         *
 * **************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>


//Main function
int main(int argc, char **argv) {
    int type1;
    int type2;
    char buf[100] = {'\0'};
    int pipe2to3[2], semid, shm_fd;
    int *shmid;
    void *shm;
    struct sembuf sem_buf;

    pipe2to3[0] = atoi(argv[1]);
    semid = atoi(argv[2]);
    //Open shared memory
    if((shm_fd = shm_open(argv[3], O_RDONLY, 0)) == -1)
        perror("C3 shm_open\n");
    if((shm = mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0)) == MAP_FAILED)
            perror("C3 mmap\n");
    //Cast to int
    shmid = (int *)shm;
    FILE *outFile = fopen(argv[4], "w");
    sem_buf = (struct sembuf){0, -1, 0};
    semop(semid, &sem_buf, 1);
    read(pipe2to3[0], &buf, 100);
    sem_buf = (struct sembuf){1, 1, 0};
    semop(semid, &sem_buf, 1);
    fprintf(outFile, "%s", buf); 
    //While true
    while (1) {
        sem_buf = (struct sembuf){0, -1, 0};
        semop(semid, &sem_buf, 1);
        if (read(pipe2to3[0], &buf, 100) < 1) {
            sem_buf = (struct sembuf){1, 1, 0};
            semop(semid, &sem_buf, 1);
            break;
        }
        sem_buf = (struct sembuf){1, 1, 0};
        semop(semid, &sem_buf, 1);
        fprintf(outFile, " %s", buf);
    }
    //Close file and pipe 2 to 3
    close(pipe2to3[0]);
    fclose(outFile);
    //Print the totals
    type1 = shmid[0];
    printf("Type 1: %d\n", type1);
    type2 = shmid[1]; 
    printf("Type 2: %d\n", type2);
    return 0;
}
