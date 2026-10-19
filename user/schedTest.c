#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


#define NUMCHILDREN 8

int
count_armstrong(int limit)
{
  int count = 0;
  for(int n = 2; n <= limit; n++) {
  
    int digits = 0, t = n;
    while(t > 0) { 
    digits++; 
    t /= 10; 
    }

    int sum = 0;
    t = n;
    while(t > 0) {
    int d = t % 10, pw = 1;
    for(int i = 0; i < digits; i++) {
      pw *= d;
    }
    sum += pw;
    t /= 10;
    }
  
    if(sum == n) {
      count++;
    }
  }
  return count;
}

int
main(int argc, char *argv[])
{
  int limit = 1000000;
  if(argc > 1) {
    limit = atoi(argv[1]);
  }
  
  char buf[16];
  printf("Enter a Debug Value (Must be either 0 or 1):\n");
  gets(buf, sizeof(buf));
  int dbg = atoi(buf);
  if(debug(dbg) < 0) {
    printf("Invalid Debug Value! Continuing with debug=1\n");
    debug(1);
  }

  int priorities[NUMCHILDREN] = {0, 0, 5, 5, 10, 10, 15, 15};
  int start = uptime();

  for(int i = 0; i < NUMCHILDREN; i++) {
    int pid = priority_fork(priorities[i]);
    if(pid < 0) {
      printf("priority_fork failed\n");
      exit(1);
    }
    if(pid == 0) {
      exit(count_armstrong(limit));
    }
    printf("\nForking a process with PID: %d and Priority: %d\n",
           pid, priorities[i]);
  }

  int pids[NUMCHILDREN], counts[NUMCHILDREN], times[NUMCHILDREN];
  for(int i = 0; i < NUMCHILDREN; i++) {
    int status;
    pids[i] = wait(&status);
    times[i] = uptime() - start;
    counts[i] = status;
  }

  debug(0);

  for(int i = 0; i < NUMCHILDREN; i++) {
    printf("\nProcess %d:\n", pids[i]);
    printf("Number of Armstrong Numbers between 2 and %d: %d\n", limit, counts[i]);
    printf("Time for process to complete:%dticks\n", times[i]);
  }
  exit(0);
}
  

