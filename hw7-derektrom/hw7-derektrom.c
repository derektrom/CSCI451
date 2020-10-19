/********************
 *Derek Trom        *
 *derek.trom@und.edu*
 *CSCI451 HW7       *
 *10/19/2020        *
 ********************
                  _.-, 
              _ .-'  / .._
           .-:'/ - - \:::::-.
         .::: '  e e  ' '-::::.
        ::::'(    ^    )_.::::::
       ::::.' '.  o   '.::::'.'/_
   .  :::.'       -  .::::'_   _.:
 .-''---' .'|      .::::'   '''::::
'. ..-:::'  |    .::::'        ::::
 '.' ::::    \ .::::'          ::::
      ::::   .::::'           ::::
       ::::.::::'._          ::::
        ::::::' /  '-      .::::
         '::::-/__    __.-::::'
           '-::::::::::::::-'
               '''::::'''
*/
//Includes
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS 5

//Global vars
const char* monsters[] = {"Vlad", "Frank", "Bigfoot", "Casper", "Gomez"};
pthread_mutex_t cond;
float sharedMem = 0.0f;
//Function declarations
int main(int argc, char** argv);
void* threadFunction(void* arg);


void* threadFunction(void* arg){
    //Variable declarations for thread function
    const char* threadName = (char*) arg;
    char fileName[12];
    char value[10];
    char withdrawDeposit;
    float badSpending;

    sprintf(fileName, "%s.in", threadName);
    FILE *in = fopen(fileName, "r");
    printf("In thread %s",threadName);

    while(fscanf(in, "%c", &withdrawDeposit) != EOF)
    {
        switch (withdrawDeposit)
        {
        case 'R':
            fscanf(in, "\n");
            pthread_mutex_lock(&cond);
            break;
        case '+':
            fscanf(in, "%f\n", &badSpending);
            sharedMem += badSpending;
            break;
        case '-':
            fscanf(in, "%f\n", &badSpending);
            sharedMem -= badSpending;
            break;
        case 'W':
           printf("Account balance after thread %s is $%.2f\n", threadName, sharedMem);
           pthread_mutex_unlock(&cond);
           fscanf(in, "\n");
           usleep(1);
           //sleep(.25);
           break;           
        default:
            printf("Unreadable command in file %s: %c\n", threadName, withdrawDeposit);
            break;
        }            
    }
     
    fclose(in);

    pthread_exit(NULL);
    
    
}

//Main Thread Function, AKA Morticia
int main(int argc, char** argv){
    printf("Hello from Morticia, say hello to my monsters!!\n");
    pthread_mutex_init(&cond, NULL);
    pthread_t threads[NUM_THREADS];
    for (int i = 0;i<NUM_THREADS;i++){
        pthread_create(&threads[i], NULL, threadFunction, (void*) monsters[i]);
        printf("Created %s thread bwahaha\n", monsters[i]);
        
    }
    for (int i = 0; i<NUM_THREADS;i++){
        pthread_join(threads[i], NULL);
        printf("%s's thread finished spending terribly...bwahaha\n", monsters[i]);
    }
    pthread_mutex_destroy(&cond);
    printf("Mortica is done, monsters account is $%.2f\n", sharedMem);

    return 0;
}

