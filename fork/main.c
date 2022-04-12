// #include <stdio.h>
// #include <stdlib.h>

// #include "kernel.h"

// int main(int argc, char *argv[]) {
//     return EXIT_SUCCESS;
// }

#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<signal.h>

void sig_handler_parent(int signum){
  printf("Parent : Received a response signal from child \n");
}

void sig_handler_child(int signum){
  char c;
  fprintf(stderr, "Child : Received a signal from parent \n");
  fprintf(stderr, "reading char:\t");
  read(0, &c, sizeof(char));
  fprintf(stderr, "recieved this: %c, using write: ", c);
  write(1, &c, sizeof(c));
  sleep(1);
  kill(getppid(),SIGUSR1);
}

int main(){
  pid_t pid;
  if((pid=fork())<0){
    printf("Fork Failed\n");
    exit(1);
  }
  /* Child Process */
  else if(pid==0){
    signal(SIGUSR1,sig_handler_child); // Register signal handler
    printf("Child: waiting for signal\n");
    pause();
  }
  /* Parent Process */
  else{
    signal(SIGUSR1,sig_handler_parent); // Register signal handler
    sleep(1);
    printf("Parent: sending signal to Child\n");
    kill(pid,SIGUSR1);
    printf("Parent: waiting for response\n");
    pause();
  }
  return 0;
}