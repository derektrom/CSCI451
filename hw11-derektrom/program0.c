/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 11 program 0 *
 * 16 November 2020         *
 * **************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>    
#include <sys/types.h>  
#include <sys/wait.h>   
#include <sys/sem.h>    
#include <errno.h>      
#include <unistd.h>    
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>

//union semaphore
union semaphoreUnion {
    int value;
    struct semid_ds *buffer;
    ushort *array;
};

//function declarations
int createSemaphore(key_t key);
int main(int argc, char **argv);


//Main driver function for all programs

int main(int argc, char **argv) {
    if (argc < 3 || argc > 3) {
        printf("Error!!!\nUsage: ./program0 <Input File Name> <Output File Name>\n");
        return 0;
    }
    printf("Etslay onvertcay otay igpay atinlay!!\n");
    //Create necessary (more or less) variables
    char *inFileName = argv[1];
    //Check if input file exists
    FILE *file;
    if ((file = fopen(inFileName, "r")) == NULL) {
        printf("No input file.\n");
        return 0;
    }
    fclose(file);
    char *outFileName = argv[2];
    char *filePath = "HW11shm";
    key_t semKey1, semKey2;
    int semid1, semid2, status, shm_fd, pipe1to2[2], pipe2to3[2];
    pid_t pid1, pid2, pid3;
    void *shmid;
    semKey1 = ftok("input.data", 'A');
    semKey2 = ftok("input.data", 'B');
    pipe(pipe1to2);
    pipe(pipe2to3);
    //Create shared memory
    if ((shm_fd = shm_open(filePath, O_CREAT | O_RDWR, 0666)) == -1) {
        perror("shm_open");
        return 0;
    }
    ftruncate(shm_fd, 4096);
    if ((shmid = mmap(0, 4096, PROT_WRITE | PROT_READ,
                    MAP_SHARED, shm_fd, 0)) == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 0;
    }
    //printf("Made it here\n");
    //Create semaphores
    if ((semid1 = createSemaphore(semKey1)) == -1) {
        perror("createSemaphore1");
        munmap(shmid, 4096);
        shm_unlink(filePath);
        return 0;
    }
    //printf("Made it here 2\n");
    if ((semid2 = createSemaphore(semKey2)) == -1) {
        perror("createSemaphore2");
        semctl(semid1, 0, IPC_RMID);
        munmap(shmid, 4096);
        shm_unlink(filePath);
        return 0;
    }
    //Convert IDs to char*
    char pipe1to2Read[3], pipe1to2Write[3];
    char pipe2to3Read[3], pipe2to3Write[3];
    char sem1Char[10], sem2Char[10], shmChar[17];
    sprintf(sem1Char, "%d", semid1);
    sprintf(sem2Char, "%d", semid2);
    //Start program1
    if ((pid1 = fork()) == 0) {
        close(pipe2to3[0]);
        close(pipe2to3[1]);
        close(pipe1to2[0]);
        sprintf(pipe1to2Write, "%d", pipe1to2[1]);
        char *args[] = {"./program1", inFileName, pipe1to2Write, sem1Char, NULL};
        execv(args[0], args);
    }
    //Start program2
    if ((pid2 = fork()) == 0) {
        close(pipe1to2[1]);
        close(pipe2to3[0]);
        sprintf(pipe1to2Read, "%d", pipe1to2[0]);
        sprintf(pipe2to3Write, "%d", pipe2to3[1]);
        char *args[] = {"./program2", pipe1to2Read, pipe2to3Write, sem1Char,
                        sem2Char, filePath, NULL};
        execv(args[0], args);
    }
    //Close pipe1to2 to prevent it from being in the memory of program3
    close(pipe1to2[0]);
    close(pipe1to2[1]);
    //Start program3 
    if ((pid3 = fork()) == 0) {
        close(pipe2to3[1]);
        sprintf(pipe2to3Read, "%d", pipe2to3[0]);
        char *args[] = {"./program3", pipe2to3Read, sem2Char, filePath, outFileName, NULL};
        execv(args[0], args);
    }
    //close pipes in program0
    close(pipe2to3[0]);
    close(pipe2to3[1]);
    //Wait for children
    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    waitpid(pid3, &status, 0);
    //Clean up shared memory and semaphores
    shm_unlink(filePath);
    semctl(semid1, 0, IPC_RMID);
    semctl(semid2, 0, IPC_RMID);
    printf("Allway oneday anslatingtray\n");
    return 0;
}
//function to create semaphore
int createSemaphore(key_t key) {
    union semaphoreUnion arg;
    struct semid_ds buffer;
    struct sembuf semBuf;
    int semaphoreNum;
    //get semaphore id 
    semaphoreNum = semget(key, 2, IPC_CREAT | IPC_EXCL | 0666);
    
    if (semaphoreNum >= 0) {
        semBuf.sem_op = 1;
        semBuf.sem_flg = 0;
        arg.value = 1;
        for (semBuf.sem_num = 0; semBuf.sem_num < 2; semBuf.sem_num++) {
            if (semop(semaphoreNum, &semBuf, 1) == -1) {
                int e = errno;
                semctl(semaphoreNum, 0, IPC_RMID);
                errno = e;
                return -1;
            }
        }
        semctl(semaphoreNum, 1, SETVAL, arg);
        arg.value = 0;
        semctl(semaphoreNum, 0, SETVAL, arg);
    } 
    return semaphoreNum;
}
