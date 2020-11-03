/*********************
 *Derek Trom         *
 *derek.trom@und.edu *
 *CSCI451 HW9        *
 *11/03/2020         *
 ********************/
/*
*When run with mutex on the threads shared the resources well.
*When it was run without the mutexes the output was still the
*same. However threads ended up hogging the resource and it
*seemed like the threads would hold onto the resource for longer
*periods of time. 
*I believe this may just be how the VM I am using is set 
*up to handle these situations. 
*/

/***********************
*Includes/defines      *
************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define NUM_THREADS 10

/***********************
*Global declarations*
************************/
pthread_mutex_t mute;
FILE * out;
FILE * inFile;

/***********************
*Structure declarations*
************************/
struct arg_struct {
    int id;
    FILE *file;
    FILE *in;
};

//mutex
void* mutexFunction(void *arg ){
    struct arg_struct *threadInfo = (struct arg_struct*)arg;
    char temp[100];
    int threadNum = threadInfo->id;
    FILE* out = (FILE*) threadInfo->file;
    FILE* fp = (FILE*) threadInfo->in;
    while (fscanf(fp, "%s\n", temp)!= EOF) {
        pthread_mutex_lock(&mute);
        fprintf(out, "%s\t%d\n", temp, threadNum);
        pthread_mutex_unlock(&mute);
        usleep(1);
    }
    pthread_exit(NULL);//exit pthread
}

//no mutex
void* noMutexFunction(void* arg){
    struct arg_struct *threadInfo = (struct arg_struct*)arg;
    char temp[100];
    int threadNum = threadInfo->id;
    FILE* out = (FILE*) threadInfo->file;
    FILE* fp = (FILE*) threadInfo->in;
    while (fscanf(fp, "%s\n", temp)!= EOF) {
        fprintf(out, "%s\t%d\n", temp, threadNum);
        //usleep(1);
    }
    pthread_exit(NULL);//exit pthread
}
int main(int argc, char** argv){
    struct arg_struct data[NUM_THREADS];
    if (argc < 2) {
        printf("Usage: \'./a.out (mutex|nomutex)\' \n");
        exit(-1);
    }
    
    //array of threads
    pthread_t threads[NUM_THREADS];
    inFile = fopen("hw9.data", "r");
    //create threads
    if (0 == strcmp(argv[1], "mutex")) {
        pthread_mutex_init(&mute, NULL);
        out = fopen("hw9.mutex", "w");
        for (int i = 0;i<NUM_THREADS;i++){
            data[i].file = out;
            
            data[i].id = i;
            data[i].in = inFile;
            pthread_create(&threads[i], NULL, mutexFunction, (void*)&data[i]);
            printf("Created %d thread\n", i); 
        }   
    
    } else {
        out  = fopen("hw9.nomutex", "w");
        for (int i = 0;i<NUM_THREADS;i++){
            data[i].file = out;
            data[i].in = inFile;
            data[i].id = i;
            pthread_create(&threads[i], NULL, noMutexFunction, (void*)&data[i]);
            printf("Created %d thread\n", i);    
        }
    }
    
    for (int i = 0; i<NUM_THREADS;i++){
        pthread_join(threads[i], NULL);
    }
    printf("Done!!\n");
    fclose(inFile);
    fclose(out);
    pthread_mutex_destroy(&mute);
    return 0;
}
