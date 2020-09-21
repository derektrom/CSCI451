/**********************
 * Derek Trom         *
 * derek.trom@und.edu *
 * CSCI451 hw3        *
 * 20200921           *
**********************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>


#define URL "http://undcemcs01.und.edu/~ronald.marsh/CLASS/CS451/hw3-data.txt"



struct threadStructure {
		char* file;
		int fileSize;
		char* search;
		int* counter;
};

void getFile();
int getFileSize(FILE *fp);
void* findWord(void* threadData);
int main(int argc, char** argv);


void getFile()
{
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
    FILE * fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("File not found! \n");
        exit(1);
    }
    
    char *buffer = calloc(sizeof(char), *sizeOfFile+1);
    
    if (!buffer) 
    {
        printf("couldn't allocate memory for file contents\n");
        exit(2);
    }
   
    int done = fread(buffer, sizeof(buffer)/sizeof(*buffer), *sizeOfFile, fp);
    if (done < 1)
    {
        printf("Something broke\n\n");
        exit(0);
    }
    fclose(fp);
    return buffer;
}

int main(int argc, char** argv)
{
    char word1[5] = "easy\0";
    char word2[6] = "polar\0";
    getFile();
    FILE * fp = fopen("hw3-data.txt", "r");
    if (fp == NULL)
    {
        printf("File not found! \n");
        exit(1);
    }
    int fileLength = getFileSize(fp);
    char* pointerArray = loadBuffer("hw3-data.txt", &fileLength);
    
    struct threadStructure *t1;
    int easyCount = 0;
    t1 = calloc(1, sizeof(struct threadStructure));
    t1->file = pointerArray;
    t1->search = word1;
    t1->fileSize = fileLength;
    t1->counter = &easyCount;
    
    struct threadStructure *t2;
    int polarCount = 0;
    t2 = calloc(1, sizeof(struct threadStructure));
    t2->file = pointerArray;
    t2->search = word2;
    t2->fileSize = t1->fileSize;
    t2->counter = &polarCount;
    
    pthread_t threadID_1;
    pthread_create(&threadID_1, NULL, findWord, (void*)t1);
    pthread_t threadID_2;
    pthread_create(&threadID_2, NULL, findWord, (void*)t2);
    
    pthread_join(threadID_1, NULL);
    pthread_join(threadID_1, NULL);
    

    printf("Count of easy: %d\n", easyCount);
    printf("Count of polar: %d\n", polarCount);
    pthread_exit(NULL);
    free(t1);
    free(t2);
    free(pointerArray);
    return 1;
}
