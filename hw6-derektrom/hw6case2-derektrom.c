/**********************
 * Derek Trom     *    
 * derek.trom@und.edu *
 * HW6 CSCI451    *
 * 12 October 2020   *
**********************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

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
void hsignal(int i);

//Main function
int main(int argc, char** argv){
  int signum;
  pthread_t threads[3];
  FILE *out = fopen("hw5.out", "w");    
  signal(SIGSEGV,hsignal); //signal handler to catch segfault
  pthread_create(&(threads[0]), NULL, &threadFunction1, NULL);
  pthread_create(&(threads[1]), NULL, &threadFunction2, NULL);
  pthread_create(&(threads[2]), NULL, &threadFunction3, NULL);
  
  while (flag < 3){
    pthread_mutex_lock(&mainMute);
    if (printVar == 1 && flag < 3){
      if(&sharedMem == '\0'){
        break;
      }
      fprintf(out, "%c\n", sharedMem);
      flag = 0; 
    }
    if (state == 1 && flag < 2){
      state++;
      pthread_mutex_unlock(&cond2);
    }else if(state == 2 && flag < 3){
      state++;
      pthread_mutex_unlock(&cond3);
    }else if(state == 3 || state == 0){
      state = 1;
      pthread_mutex_unlock(&cond1);
    }else{
      break;
    }
    
  }
  for(int i = 0; i<NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
  }
  pthread_mutex_destroy(&cond1);
  pthread_mutex_destroy(&cond2);
  pthread_mutex_destroy(&cond3);
  pthread_mutex_destroy(&mainMute);
  return 0;
}
void* threadFunction1(void* arg){
  int i = 1;
	FILE *in = fopen("hw5-1.in", "r");

	while(i == 1 && flag == 0){
	  pthread_mutex_lock(&cond1);

	  if(state == 1){
	    i = fscanf(in, "%c\n", &sharedMem);

	    if(i == -1 || sharedMem == '\0'){
        flag++;
        fclose(in);
	      state++;
	      pthread_mutex_unlock(&cond2);
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
void* threadFunction2(void *arg){
	int i = 1;
	FILE *in = fopen("hw5-2.in", "r");

	while(i == 1 && flag < 2){
	  pthread_mutex_lock(&cond2);

	  if(state == 2){
	    i = fscanf(in, "%c\n", &sharedMem);
        
        if (sharedMem == '&'){
          printf("Case 2: I am killing a child\n");
          int pid = getpid();
          char buffer[32];
          pthread_kill(pthread_self(), SIGSEGV);
          //sprintf(buffer,"kill -s 9 %d",pid);
          //system(buffer);
        }
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
//signal handler. 
void hsignal(int i)
{
  printf("Signal %i\\n",i);
  pthread_exit(0);
}
