/****************************
 * Derek Trom               *
 * derek.trom@und.edu       *
 * CSCI 451 HW 10 program 2 *
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

//Function declarations
int checkVowel(char letter);
int checkLetter(char letter);
int main(int argc, char **argv);

//Main
int main(int argc, char **argv) {
    //put EOF symbol in the buffer
    char buffer[100] = {'\0'};
    int pipe1to2[2];
    int pipe2to3[2];
    int semid;
    int end;
    int type1Count = 0;
    int type2Count = 0;
    struct sembuf unlock;
    struct sembuf lock;
    //Create locks and unlocks for 
    unlock.sem_num = 0;
    unlock.sem_op = 1;
    unlock.sem_flg = 0;
    lock.sem_num = 0;
    lock.sem_op = -1;
    lock.sem_flg = 0;
    //Convert back to integers
    pipe1to2[0] = atoi(argv[1]);
    pipe1to2[1] = atoi(argv[2]);
    semid = atoi(argv[3]);
    pipe2to3[0] = atoi(argv[4]);
    pipe2to3[1] = atoi(argv[5]);
    //Make pipe not block
    fcntl(pipe1to2[0], F_SETFL, O_NONBLOCK);
    //close pipes
    close(pipe1to2[1]);
    close(pipe2to3[0]);
    while (1) {
        semop(semid, &lock, 1);
        //Check if read returns EOF or error
        if ((end = read(pipe1to2[0], &buffer, 100)) <= 0) {
            //Check if EOF reached
            if (end == 0) {
                semop(semid, &unlock, 1);
                break;
            //Read would've been blocked so release semaphore and continue
            } else if (errno == EAGAIN) {
                semop(semid, &unlock, 1);
                continue;
            }
        }
        semop(semid, &unlock, 1);
        int bufferSize = strlen(buffer);
        if (checkVowel(buffer[0])) {
            //if it is a vowel it will have 'ray'
            type1Count += 1;
            if (checkLetter(buffer[bufferSize - 1])) {
                //if it is a letter add ray
                strcat(buffer, "ray");
            } else {
                //else it is a period or a comma
                if (buffer[bufferSize - 1] == ',') {
                    buffer[bufferSize - 1] = '\0';
                    strcat(buffer, "ray,");
                } else {
                    buffer[bufferSize - 1] = '\0';
                    strcat(buffer, "ray.");
                }
            }
        } else { //it is type 2
            type2Count += 1;
            char buffer2[100] = {'\0'}; 
            if (checkLetter(buffer[bufferSize -1])) {
                //If letter then switch the word
                strcpy(buffer2, buffer+1);
                buffer2[bufferSize - 1] = buffer[0];
                strcat(buffer2, "ay");
                strcpy(buffer, buffer2);
            } else {
                //else move other way
                strcpy(buffer2, buffer+1);
                buffer2[bufferSize - 2] = buffer[0];
                if (buffer[bufferSize - 1] == ',') {
                    //if comma put comma after ay
                    strcat(buffer2, "ay,");
                } else {
                    //else it is a period
                    strcat(buffer2, "ay.");
                }
                strcpy(buffer, buffer2);
            }
        }
        write(pipe2to3[1], &buffer, 100);
    }
    //Close first pipe and semaphore
    close(pipe1to2[0]);
    //free semaphore
    semctl(semid, 0, IPC_RMID);
    //Write to the files
    FILE *out1 = fopen("shared1.dat", "w");
    FILE *out2 = fopen("shared2.dat", "w");
    fprintf(out1, "%d\n", type1Count);
    fprintf(out2, "%d\n", type2Count);
    //close files

    fclose(out1);
    fclose(out2);
    //Close pipe here so from 2 to 3
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
