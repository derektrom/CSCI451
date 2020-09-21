/**********************
 * Derek Trom         *
 * derek.trom@und.edu *
 * CSCI451 hw3        *
 * 20200921           *
**********************/
//library imports
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>

//define the url
#define URL "http://undcemcs01.und.edu/~ronald.marsh/CLASS/CS451/hw3-data.txt"


//structure for thread info
struct threadStructure {
		char* file;
		int fileSize;
		char* search;
		int* counter;
};

//function declarations
void getFile();
int getFileSize(FILE *fp);
void* findWord(void* threadData);
int main(int argc, char** argv);


void getFile()
{
    /*
     * get file using wget
     * Return Value: void
     */
    FILE * fp = fopen("hw3-data.txt", "r");
    if (fp != NULL)
    {
        fclose(fp);
        system("rm hw3-data.txt");
    }
    system("wget -q " URL);
}

int getFileSize(FILE *fp)
{
    /*
     * Calculate file size
     * Return Value: int sizeOfFile
     */
    int sizeOfFile = 0;
    int a;
    do {
          a = fgetc(fp);
          if (feof(fp))
          {
              break;
          }
          sizeOfFile++;
      }while (1);
    return sizeOfFile;
}
void* findWord(void* dataStruct)
{
    /*
     * find occurrences of word
     * Return Value: void pointer to update count in structure
     */
    struct threadStructure* data = (struct threadStructure*)dataStruct;
    int wordlen = strlen(data->search);
    int letterIndex = 0;
    int in_word = 0;
    int i;
    for (i = 0; i < data->fileSize; i++) 
    {
        data->file[i] = tolower(data->file[i]);
        if(data->file[i] == data->search[letterIndex])
        {
            ++letterIndex;
            if(letterIndex == wordlen)
            {
                // match
                (*data->counter)++;
                letterIndex = 0;
            }
        }
        else
        {
           i -=letterIndex;
           letterIndex = 0;
        }
    } 
}

char* loadBuffer(char* filename, int* sizeOfFile)
{
    /*
     * load up character value from file
     * Return value: buffer
     */

    //open file
    FILE * fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("File not found! \n");
        exit(1);
    }
    //allocate contiguous memory
    char *buffer = calloc(sizeof(char), *sizeOfFile+1);
    
    if (!buffer) 
    {
        printf("couldn't allocate memory for file contents\n");
        exit(2);
    }
    //use fread to load the buffer from file
    int done = fread(buffer, sizeof(buffer)/sizeof(*buffer), *sizeOfFile, fp);
    if (done < 1)
    {
        printf("Something broke\n\n");
        exit(0);
    }
    //close file
    fclose(fp);
    return buffer;
}

int main(int argc, char** argv)
{
    //words to search for with null terminator
    char word1[5] = "easy\0";
    char word2[6] = "polar\0";
    //get the file
    getFile();
    //open downloaded file
    FILE * fp = fopen("hw3-data.txt", "r");
    if (fp == NULL)
    {
        printf("File not found! \n");
        exit(1);
    }
    //get size of file
    int fileLength = getFileSize(fp);
    //load the buffer pointerArray
    char* pointerArray = loadBuffer("hw3-data.txt", &fileLength);
    //create structure t1
    struct threadStructure *t1;
    int easyCount = 0;
    t1 = calloc(1, sizeof(struct threadStructure));
    t1->file = pointerArray;
    t1->search = word1;
    t1->fileSize = fileLength;
    t1->counter = &easyCount;
    //create structure t2
    struct threadStructure *t2;
    int polarCount = 0;
    t2 = calloc(1, sizeof(struct threadStructure));
    t2->file = pointerArray;
    t2->search = word2;
    t2->fileSize = t1->fileSize;
    t2->counter = &polarCount;
    //create and start threadID_1
    pthread_t threadID_1;
    pthread_create(&threadID_1, NULL, findWord, (void*)t1);
    //create and start threadID_2
    pthread_t threadID_2;
    pthread_create(&threadID_2, NULL, findWord, (void*)t2);
    //join threads
    pthread_join(threadID_1, NULL);
    pthread_join(threadID_1, NULL);
    
    //print end result of search
    printf("Count of easy: %d\n", easyCount);
    printf("Count of polar: %d\n", polarCount);
    //exit pthreads
    pthread_exit(NULL);
    //free allocated memory
    free(t1);
    free(t2);
    free(pointerArray);
    return 1;
}
