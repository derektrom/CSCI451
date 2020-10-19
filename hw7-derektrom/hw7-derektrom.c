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

//thread function
void* threadFunction(void* arg){
    //Variable declarations for thread function
    const char* threadName = (char*) arg;
    char fileName[10];
    char withdrawDeposit;
    float badSpending;

    sprintf(fileName, "%s.in", threadName);
    FILE *in = fopen(fileName, "r");
    while(fscanf(in, "%c", &withdrawDeposit) != EOF)
    {
        //switch statement for enter critical section, deposit
        //withdraw or exit critical section
        switch (withdrawDeposit)
        {
        case 'R':
            fscanf(in, "\n");
            //lock the mutex
            pthread_mutex_lock(&cond);
            break;
        case '+':
            fscanf(in, "%f\n", &badSpending);
            //deposit money
            sharedMem += badSpending;
            break;
        case '-':
            fscanf(in, "%f\n", &badSpending);
            //withdraw money
            sharedMem -= badSpending;
            break;
        case 'W':
           printf("Account balance after thread %s is $%.2f\n", threadName, sharedMem);
           fscanf(in, "\n");
           //unlock mutex
           pthread_mutex_unlock(&cond);
           //sleep 1/4 second to create randomness
           sleep(.25);
           break;           
        default:
            //default if the wrong symbol is in file
            printf("Unreadable command in file %s: %c\n", threadName, withdrawDeposit);
            break;
        }            
    }
    //close file when finished 
    fclose(in);
    //exit thread
    pthread_exit(NULL);
    
    
}

//Main Thread Function, AKA Morticia
int main(int argc, char** argv){
    printf("Hello from Morticia, say hello to my monsters!!\n");
    //initialize mutex
    pthread_mutex_init(&cond, NULL);
    //array of threads
    pthread_t threads[NUM_THREADS];
    //create threads
    for (int i = 0;i<NUM_THREADS;i++){
        pthread_create(&threads[i], NULL, threadFunction, (void*) monsters[i]);
        printf("Created %s thread bwahaha\n", monsters[i]);    
    }
    //join threads
    for (int i = 0; i<NUM_THREADS;i++){
        pthread_join(threads[i], NULL);
        printf("%s's thread finished spending terribly...bwahaha\n", monsters[i]);
    }
    pthread_mutex_destroy(&cond);
    printf("Mortica is done, monsters account is $%.2f\n", sharedMem);

    return 0;
}

