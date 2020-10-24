/*********************
 *Derek Trom         *
 *derek.trom@und.edu *
 *CSCI451 HW8        *
 *10/26/2020         *
 ********************/

/***********************
*Includes/defines      *
************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS 5
#define NUM_CLASSES 6
#define NUM_PAGES 6

/***********************
*Global declarations*
************************/
const char* monsters[] = {"Vlad", "Frank", "Bigfoot", "Casper", "Gomez"};
pthread_mutex_t mute;
float sharedMem = 0.0f;
int finished = 0;

/***********************
*Structure declarations*
************************/
struct page *head = NULL;
struct page {
    int id;
    int R;
    int M;
    int class;
    int page;
    struct page *next;
};

struct queue {
	unsigned int randState;
    const char *threadName;
    int id;
    int *finished;    // current number of items
	struct page *nodes[6];
    pthread_mutex_t queueMute;
};


/*********************
Function declarations*
**********************/
int main(int argc, char** argv);
void* threadFunction(void* arg);
void create_queue(struct page **head);
void delete_queue(struct page **head);
int request(int owner);
int update(int rreq, int wreq, float balance, int page, int owner);

/***********************
*Update bits function  *
************************/
int update(int rreq, int wreq, float balance, int page, int owner)
{
    int i = 0;

    struct page *n = head; // point to start of list

    for (i = 0; i < NUM_PAGES; i++)
    {
        if (n->page == page && n->id == owner)
        {
            // read request
            if (rreq == 1)
            {
                n->R = 1;
            }

            // write request
            else if (wreq == 1)
            {
                // negative balance
                if (balance < 0)
                {
                    n->R = 1;
                    n->M = 1;

                }

                // positive balance
                else if(balance > 0)
                {
                    n->R = 1;
                }
            }

            // set r = to 0  10 % of time
            if (rand() % 10 == 0)
            {
                n->R = 0;
            }

        return page; // return same page
        } // end if

        n = n->next; // next node
    } // end for
    return request(owner);

}

/***********************
*Create buffer         *
************************/
void create_queue(struct page **head){

	struct page *start = (struct page*)malloc(sizeof(struct page));
    start->R = 0;
    start->M = 0;
    start->id = -1;
    start->page = 0;
    start->next = NULL;
    //pthread_mutex_init(&start->pagemute, NULL);
    (*head) = start;
    for (int i = 0; i < NUM_THREADS; i++)
    {
		// populate the variables of the queue :
		struct page *nextNode = (struct page*)malloc(sizeof(struct page));
        start->next = nextNode;
        nextNode->R = 0;
        nextNode->M = 0;
        nextNode->id = -1;
        nextNode->next = NULL;
        nextNode->page = i+1;
        //pthread_mutex_init(&nextNode->pagemute, NULL);
        start = nextNode; 
    }
    start->next = (*head);
}
/***********************
*Delete queue          *
************************/
void delete_queue(struct page **head){
    struct page *last = (*head);
    struct page *next = last->next;
    for (int i = 0; i < NUM_THREADS-1; i++)
    {
        last->next = NULL;
        free(last);
        last = next;
        next = next->next;
    }
    last->next = NULL;
    free(last);
    next->next = NULL;
    free(next);
}


/***********************
*Thread function       *
************************/
void* threadFunction(void* arg){
    //Variable declarations for thread function
    struct queue *threadInfo = (struct queue*)arg;
    char fileName[10];
    char withdrawDeposit;
    float badSpending;
    int page = 0;
    int id = threadInfo->id;

    sprintf(fileName, "%s.in", threadInfo->threadName);
    FILE *in = fopen(fileName, "r");
    page = request(id);
    printf("inital page: %d\n", page);
    while(fscanf(in, "%c", &withdrawDeposit) != EOF)
    {
        //switch statement for enter critical section, deposit
        //withdraw or exit critical section
        switch (withdrawDeposit)
        {
        case 'R':
            fscanf(in, "\n");
            //lock the mutex
            pthread_mutex_lock(&mute);
            if (rand() % 5 != 0){//random reset of M
                page = update(1, 0, sharedMem,page,id);
            }
            else{
                page = update(1, 0, sharedMem, -1, id);
            }
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
           printf("Account balance after thread %s is $%.2f\n", threadInfo->threadName, sharedMem);
           fscanf(in, "\n");
           //unlock mutex
           pthread_mutex_unlock(&mute);
           // randomly reset R bit
           if(rand() % 5 != 0){
               page = update(0, 1, sharedMem,page,id);
           }
           // random reset of rbit
           else{
               page = update(0, 1, sharedMem, -1, id);
           }
           //sleep 1/4 second to create randomness
           sleep(.25);
           break;           
        default:
            //default if the wrong symbol is in file
            printf("Unreadable command in file %s: %c\n", threadInfo->threadName, withdrawDeposit);
            break;
        }            
    }
    //close file when finished 
    fclose(in);
    finished ++;
    //exit thread
    pthread_exit(NULL);    
}

/***********************
*Request Page Function *
************************/
int request(int owner){
    
    struct page *n = head; // point to start of list
    int i = 0;
    int j = 0;

    // look for non-used page
    for (i = 0; i < NUM_PAGES; i++)
    {
        // nonused page available
        if (n->id == -1)
        {
            n->id = owner;
            return n->page;
        }
         
        n = n->next; // next node
    }

    for (j = 0; j < NUM_CLASSES; j++)
    {
        n = head;

        // look for lowest class
        for (i = 0; i < NUM_PAGES; i++)
        { // calculate class
            if (n->R == 0 && n->M == 0) n->class = 0;
            if (n->R == 0 && n->M == 1) n->class = 1;
            if (n->R == 1 && n->M == 0) n->class = 2;
            if (n->R == 1 && n->M == 1) n->class = 3;

            // lowest class found
            if (n->id != owner && n->class == j)
            {
                printf("Page fault in thread %d ", owner);
                printf("Removing page %d owned by thread %d r= %d m = %d\n",
                    n->page, n->id, n->R, n->M);
                n->R = 0; // reiniialize
                n->M = 0;
                n->id = owner;
                return n->page;
            }

            n = n->next;

        }
    } 
    return -1;

}

/***********************
*MAIN aka Morticia     *
************************/
int main(int argc, char** argv){
    printf("Hello from Morticia, say hello to my monsters!!\n");
    struct queue data[NUM_THREADS];
    //struct page *head = NULL;
    create_queue(&head);
    struct page *page = head;
    //initialize mutex
    pthread_mutex_init(&mute, NULL);
    //array of threads
    pthread_t threads[NUM_THREADS];
    //create threads
    for (int i = 0;i<NUM_THREADS;i++){
        data[i].threadName = monsters[i];
        data[i].finished = &finished;
        data[i].id = i;
        pthread_create(&threads[i], NULL, threadFunction, (void*) &data[i]);
        printf("Created %s thread bwahaha\n", monsters[i]);    
    }
    //detach threads
    for (int i = 0; i<NUM_THREADS;i++){
        pthread_detach(threads[i]);
    }
    while(finished < NUM_THREADS);//wait until all threads finish
    pthread_mutex_destroy(&mute);
    printf("Mortica is done, monsters account is $%.2f\n", sharedMem);
    delete_queue(&head);
    return 0;
}

