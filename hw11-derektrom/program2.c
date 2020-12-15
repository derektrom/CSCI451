/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 11 program 2 *
 * 16 November 2020         *
 * **************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/shm.h>

//Function declarations
int checkVowel(char letter);
int checkLetter(char letter);
int main(int argc, char **argv);

//Main
int main(int argc, char **argv) {
    //put EOF symbol in the buffer
    char buffer[100] = {'\0'};
    char tempChar[2] = {'\0'};
    int pipe1to2[2];
    int pipe2to3[2];
    int semid1; 
    int semid2;
    int res; 
    int shm_fd;
    int stringLen, type1 = 0, type2 = 0;
    struct sembuf sem_buf;
    int *shmid;
    void *shm;
    //Convert pipe and sem IDs back to int's
    pipe1to2[0] = atoi(argv[1]);
    semid1 = atoi(argv[3]);
    pipe2to3[1] = atoi(argv[2]);
    semid2 = atoi(argv[4]);
    //Open shared memory
    if ((shm_fd = shm_open(argv[5], O_RDWR, 0)) == -1)
        perror("C2 shm_open\n");
    if ((shm = mmap(0, 4096, PROT_WRITE, MAP_SHARED, shm_fd, 0)) == MAP_FAILED) 
        perror("mmap");
    shmid = (int *)shm;
    //WHile true trasnlate stuff
    while (1) {
        sem_buf = (struct sembuf){0, -1, 0};
        semop(semid1, &sem_buf, 1);
        //If EOF break
        if ((res = read(pipe1to2[0], &buffer, 100)) < 1) {
            sem_buf = (struct sembuf){0, 1, 0};
            semop(semid2, &sem_buf, 1);
            break;
        }
        sem_buf = (struct sembuf){1, 1, 0};
        semop(semid1, &sem_buf, 1);
        stringLen = strlen(buffer);
        if (checkVowel(buffer[0])) {
            type1 += 1;
            if (checkLetter(buffer[stringLen - 1]))
                strcat(buffer, "ray");
            else {
                tempChar[0] = buffer[stringLen-1];
                buffer[stringLen-1] = '\0';
                strcat(buffer, "ray");
                strcat(buffer, tempChar);
            }
        } else {
            type2 += 1;
            char buffer2[100] = {'\0'}; 
            if (checkLetter(buffer[stringLen -1])) {
                strcpy(buffer2, buffer+1);
                buffer2[stringLen - 1] = buffer[0];
                strcat(buffer2, "ay");
                strcpy(buffer, buffer2);
            } else {
                tempChar[0] = buffer[stringLen - 1];
                buffer[stringLen - 1] = '\0';
                strcpy(buffer2, buffer+1);
                buffer2[stringLen - 2] = buffer[0];
                strcat(buffer2, "ay");
                strcat(buffer2, tempChar);
                strcpy(buffer, buffer2);
            }
        }
        sem_buf = (struct sembuf){1, -1, 0};
        semop(semid2, &sem_buf, 1);
        write(pipe2to3[1], &buffer, 100);
        sem_buf = (struct sembuf){0, 1, 0};
        semop(semid2, &sem_buf, 1);
    }
    //Close pipe1to2 and sem
    close(pipe1to2[0]);
    //share number of types via file
    shmid[0] = type1;
    shmid[1] = type2;
    //Close pipe 2 to 3
    close(pipe2to3[1]);

    return 0;
}
//check if it is a vowel
int checkVowel(char letter) {
    if (letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u'||
        letter == 'A' || letter == 'E' || letter == 'I' || letter == 'O' || letter == 'U') {
        return 1;
    }
    return 0;
}
//check if it is a letter
int checkLetter(char letter) {
    if (((letter >= 'a') && (letter <= 'z')) || ((letter >= 'A') && (letter <= 'Z'))) {
        return 1;
    }
    return 0;
}
