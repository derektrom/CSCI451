/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 10 program 0 *
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
    key_t key;
    int semaphoreNum, status;
    int pipe1to2[2];
    int pipe2to3[2];
    //pid declarations
    pid_t program1, program2, program3;    
    key = ftok("input.data", 'A');
    pipe(pipe1to2);
    pipe(pipe2to3);
    if ((semaphoreNum = createSemaphore(key)) == -1) {
        perror("createSemaphore");
        return 0;
    }
    //Convert pipe and sem IDs to char*
    char in12[3], out12[3];
    char in23[3], out23[3];
    char semChar[10];
    //write to pipes
    sprintf(in12, "%d", pipe1to2[0]);
    sprintf(out12, "%d", pipe1to2[1]);
    sprintf(in23, "%d", pipe2to3[0]);
    sprintf(out23, "%d", pipe2to3[1]);
    sprintf(semChar, "%d", semaphoreNum);
    printf("Anslatingtray ethay inputway.ataday\n");
    //fork and start program1
    if ((program1 = fork()) == 0) {
        char *args[] = {"./program1", in12, out12, semChar, NULL};
        execv(args[0], args);
    }
    //fork and start program2
    if ((program2 = fork()) == 0) {
        char *args[] = {"./program2", in12, out12, semChar,
                        in23, out23, NULL};
        execv(args[0], args);
    }
    //Close pipe1to2 
    close(pipe1to2[0]);
    close(pipe1to2[1]);
    //fork and start program3 
    if ((program3 = fork()) == 0) {
        char *args[] = {"./program3", in23, out23, NULL};
        execv(args[0], args);
    }
    
    //close pipes in program0, its not using them
    close(pipe2to3[0]);
    close(pipe2to3[1]);
    waitpid(program1, &status, 0);
    waitpid(program2, &status, 0);
    waitpid(program3, &status, 0);
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
    semaphoreNum = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    
    if (semaphoreNum >= 0) {
        semBuf.sem_op = 1;
        semBuf.sem_flg = 0;
        arg.value = 1;
        if (semop(semaphoreNum, &semBuf, 1) == -1) {
            int error = errno;
            //remove semaphore
            semctl(semaphoreNum, 0, IPC_RMID);
            errno = error;
            return -1;
        }
        semctl(semaphoreNum, 0, SETVAL, arg);
    } 
    return semaphoreNum;
}
