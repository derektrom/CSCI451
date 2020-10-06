/**********************
 * Derek Trom     *    
 * derek.trom@und.edu *
 * HW5 CSCI451    *
 * 5 October 2020   *
**********************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define NUM_THREADS 3
//Global vars
pthread_mutex_t cond1;
pthread_mutex_t cond2;
pthread_mutex_t cond3;
pthread_mutex_t mainMute;
char sharedMem;
int state;
int flag;
int printVar;

//Declare functions
void* threadFunction1(void* arg);
void* threadFunction2(void* arg);
void* threadFunction3(void* arg);
int main(int argc, char**argv);

//Main function
int main(int argc, char** argv){
  pthread_t threads[3];
  FILE *out = fopen("hw5.out", "w");    
  //create threads
  pthread_create(&(threads[0]), NULL, &threadFunction1, NULL);
  pthread_create(&(threads[1]), NULL, &threadFunction2, NULL);
  pthread_create(&(threads[2]), NULL, &threadFunction3, NULL);
  //while threads are not finished
  while (flag < 3){
    pthread_mutex_lock(&mainMute);//lock main
    if (printVar == 1 && flag < 3){
      if(&sharedMem == '\0'){//eof
        break;
      }
      fprintf(out, "%c\n", sharedMem);
      flag = 0; 
    }
    if (state == 1 && flag < 2){
      state++;
      //unlock for thread 2
      pthread_mutex_unlock(&cond2);
    }else if(state == 2 && flag < 3){
      state++;
      //unlock for thread 3
      pthread_mutex_unlock(&cond3);
    }else if(state == 3 || state == 0){
      state = 1;
      //unlock for thread 1
      pthread_mutex_unlock(&cond1);
    }else{
      break;
    }
    
  }
  //join all threads
  for(int i = 0; i<NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
  }
  //destroy stuff
  pthread_mutex_destroy(&cond1);
  pthread_mutex_destroy(&cond2);
  pthread_mutex_destroy(&cond3);
  pthread_mutex_destroy(&mainMute);
  return 0;
}

//thread 1 function
void* threadFunction1(void* arg){
  int i = 1;
  FILE *in = fopen("hw5-1.in", "r");
  while(i == 1 && flag == 0){
    pthread_mutex_lock(&cond1);//lock for 1

	if(state == 1){
	  i = fscanf(in, "%c\n", &sharedMem);//write

	  if(i == -1 || sharedMem == '\0'){
        flag++;
        fclose(in);
	    state++;
        pthread_mutex_unlock(&cond2);
	    return NULL;
      }else{
	    printVar = 1;
	    pthread_mutex_unlock(&mainMute);//let main read
      }
	}
  }

  fclose(in);
  return NULL;
}

//thread function 2
void* threadFunction2(void *arg){
	int i = 1;
	FILE *in = fopen("hw5-2.in", "r");

	while(i == 1 && flag < 2){
	  pthread_mutex_lock(&cond2);//lock for 2
      //state is two
	  if(state == 2){
	    //write for 2
        i = fscanf(in, "%c\n", &sharedMem);
        //done
	    if(i == -1 || sharedMem == '\0'){
	      flag++;
	      fclose(in);
	      state++;
	      pthread_mutex_unlock(&cond3);
	      return NULL;
	    }else{
	      printVar = 1;
	      pthread_mutex_unlock(&mainMute);
	    }
	  }
  }

	fclose(in);
	return NULL;
}

//thread function 3
void* threadFunction3(void *arg)
{
	int i = 1;
	FILE *in = fopen("hw5-3.in", "r");

	while(i == 1 && flag < 3){
	  pthread_mutex_lock(&cond3);

	  if(state == 3){
	    i = fscanf(in, "%c\n", &sharedMem);
      if(i == -1 || sharedMem == '\0'){
	      flag++;
	      fclose(in);
	      state++;
	      pthread_mutex_unlock(&mainMute);
	      return NULL;
	    }else{
	      printVar = 1;
	      pthread_mutex_unlock(&mainMute);
	    }
	  }
	}

	fclose(in);
	return NULL;
}
