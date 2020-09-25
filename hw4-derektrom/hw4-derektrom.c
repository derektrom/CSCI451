/***********************
 * Derek Trom          *
 * derek.trom@und.edu  *
 * CSCI451 HW4         *
 * 20200928            *
 * ********************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 

//Global Vars
int sharedNum; //shared memory
pthread_mutex_t mutex;//declaration of mutex for locking and unlocking shared mem
pthread_cond_t cond; //signal condition

int valueRead = 0;//control for critical area
int eof = 0;//finished with file

//function declarations
void* readFile(void* param);
int main(int argc, char** argv);
void* writeFile(void* param);
int cursorStuff();

void* readFile(void* param)
{
    /*
     * thread function for reading in file
     */
    int readValue;//temp value for int to write to sm
    FILE* in = fopen("hw4.in", "r");//open file for reading
    if (in == NULL)
    {
        printf("File not found! \n");
        exit(1);
    }

    while (fscanf(in, "%d\n", &readValue) != EOF)
    {
        pthread_mutex_lock(&mutex);//in critical section lock
        while (valueRead == 1)
        {
            //puts("Waiting on consumer");
            pthread_cond_wait(&cond, &mutex);
        }
        sharedNum = readValue;//write to sm
        valueRead = 1;//value read
        pthread_cond_signal(&cond);//signal that it is done
        pthread_mutex_unlock(&mutex);//unlock
    }
    pthread_mutex_lock(&mutex);//reached end of file, lock
    while (valueRead == 1) 
    {
        //puts("Waiting on consumer");
        pthread_cond_wait(&cond, &mutex);
    }
    eof = 1;//done
    valueRead = 1;
    pthread_cond_signal(&cond);//signal done
    pthread_mutex_unlock(&mutex);
    fclose(in);//close file
    pthread_exit(NULL);//exit thread
}

void* writeFile(void* param)
{
    /*
     * thread function to write out to file from
     * shared memory
     */
    int writeVal;//temp value for sm
    FILE* out = (FILE*) param;//passed in file from void pointer
    while(1){
        pthread_mutex_lock(&mutex);//lock sm
        while (valueRead == 0)
        {
            //puts("Waiting on producer");
            pthread_cond_wait(&cond, &mutex);
        }
        if (eof)
        {
            //puts("\nRead last int from shared memory\n");
            pthread_mutex_unlock(&mutex);//done
            break;
        }
        writeVal = sharedNum;//read from sm
        valueRead = 0;//done
        pthread_cond_signal(&cond);//signal done
        pthread_mutex_unlock(&mutex);//unlock
        if ((writeVal % 2) == 0)
            {
                fprintf(out, "%d\n%d\n", writeVal, writeVal);
            }
        else
            {
                fprintf(out, "%d\n", writeVal);
            }
    }
    pthread_exit(NULL);//exit pthread
}
int cursorStuff()
{
    printf("processing... |");
    while(eof == 0) {
        fflush(stdout);
        sleep(.05);
        printf("\b/");
        fflush(stdout);
        sleep(.05);
        // do some more
        printf("\b-");
        fflush(stdout);
        sleep(.05);
        printf("\b\\");
        fflush(stdout);
        sleep(.05);
        printf("\b|");
        fflush(stdout);
        sleep(.05);
        
    }
    
    return 0;
}

int main(int argc, char** argv)
{
    FILE * out = fopen("hw4.out", "w");
    pthread_t threads[2];//array of 2 threads
    
    pthread_cond_init(&cond, NULL);//init condition
    //start threads
    pthread_create(&threads[0], NULL, readFile, NULL);
    pthread_create(&threads[1], NULL, writeFile, (void*)out);
    //fun stuff
    cursorStuff();
    //join all threads
    for (int i = 0; i<2; i++)
    {
        pthread_join(threads[i], NULL);
    }
    //destroy cond and mutex
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    fclose(out);//close file
    puts("\n\nAll done!!");
    return 1;
}

