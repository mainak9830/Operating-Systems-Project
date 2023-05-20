
// Mainak Adak

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#define TRUE 1
#define FALSE 0

// tokenize the command string into arguments - do not modify
void readCmdTokens(char* cmd, char** cmdTokens) {
  cmd[strlen(cmd) - 1] = '\0'; // drop trailing newline
  int i = 0;
  cmdTokens[i] = strtok(cmd, " "); // tokenize on spaces
  while (cmdTokens[i++] && i < sizeof(cmdTokens)) {
    cmdTokens[i] = strtok(NULL, " ");
  }
}

// read one character of input, then discard up to the newline - do not modify
char readChar() {
  char c = getchar();
  while (getchar() != '\n');
  return c;
}

void parallel_execution(char* cmdTokens[], int count, int timeout){
    int pid = getpid();
    int lastProcess;
    printf("current starting parent process id %d\n", pid);
    //sleep(1);
    time_t start = time(0);
    while(count >= 1){
      int cpid = fork();
      
      if(cpid == -1){
	printf("Process forking falied\n");
	exit(1);
      }
      else if(cpid == 0){
	//child process
	printf("hey child!!%d %d\n", getpid(), count);
	sleep(1);
	execvp(cmdTokens[0], cmdTokens);
	
	printf("Can't execute %s\n", cmdTokens[0]); // only reached if running the program failed
	printf("Exiting as process %d\n", getpid()); 
	exit(1);
      }
      else{
	
	//printf("Same parent process %d child - %d \n",getpid(),cpid);
        double timetaken = difftime(time(0), start);
	if(timeout != 0 && timetaken > timeout){
	  //kill(cpid, SIGKILL);
	  printf("Timeout exceeded - %d\n", cpid);
	}
	//sleep(2);
	
      }
      lastProcess = cpid;
      count--;
    }
    waitpid(lastProcess, 0, 0);
}

void handler(int sig){
  printf("timeout");
}

void seq_execution(char* cmdTokens[], int count, int timeout){
    int pid = getpid();
    
    printf("current starting parent process id %d\n", pid);
    //sleep(1);
    while(count >= 1){
      
      int cpid = fork();
      
      if(cpid == -1){
	printf("Process forking falied\n");
	exit(1);
      }
      else if(cpid == 0){
	//child process
	printf("hey child!!%d %d\n", getpid(), count);
        
	execvp(cmdTokens[0], cmdTokens);
	
	printf("Can't execute %s\n", cmdTokens[0]); // only reached if running the program failed
	printf("Exiting as process %d\n", getpid()); 
	exit(1);
      }
      else{
	time_t start = time(0);
	while(waitpid(cpid, NULL, WNOHANG) == 0) {
	  double timetaken = difftime(time(0), start);

	  if(timeout != 0 && timetaken > timeout){
	    kill(cpid, SIGKILL);
	    printf("Timeout exceeded - %d\n", cpid);
	    break;
	  }
	}
	
      }
     
      count--;
    }
}

// main method - program entry point
int main() {
  char cmd[81]; // array of chars (a string)
  char* cmdTokens[20]; // array of strings
  int count; // number of times to execute command
  int parallel; // whether to run in parallel or sequentially
  int timeout; // max seconds to run set of commands (parallel) or each command (sequentially)

  while (TRUE) { // main shell input loop

    // begin parsing code - do not modify
    printf("closh> ");
    fgets(cmd, sizeof(cmd), stdin);
    if (cmd[0] == '\n') continue;
    readCmdTokens(cmd, cmdTokens);
    do {
      printf("  count> ");
      count = readChar() - '0';
    } while (count <= 0 || count > 9);
    printf("  [p]arallel or [s]equential> ");
    parallel = (readChar() == 'p') ? TRUE : FALSE;
    do {
      printf("  timeout> ");
      timeout = readChar() - '0';
    } while (timeout < 0 || timeout > 9);
    // end parsing code
    

    ////////////////////////////////////////////////////////
    //                                                    //
    // TODO: use cmdTokens, count, parallel, and timeout  //
    // to implement the rest of closh                     //
    //                                                    //
    // /////////////////////////////////////////////////////

    // just executes the given command once - REPLACE THIS CODE WITH YOUR OWN
    //execvp(cmdTokens[0], cmdTokens); // replaces the current process with the given program
    
    //parallel or sequential execution
    parallel ? parallel_execution(cmdTokens, count, timeout) : seq_execution(cmdTokens, count, timeout);
    
   
    
  }
}

