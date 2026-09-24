#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  printf("Process Uptime: %d\n", uptime());
  printf("Testing syscalltrace() implementation...\n");

  int parent_val = syscalltrace();   // returns 1 (tracing now on)

  sbrk(0);
  getpid();
  pause(1);       // use sleep(1) if your version still calls it sleep

  int pid = fork();
  if(pid == 0){
    // child: if tracing were inherited, this would print a trace line
    getpid();
    exit(0);
  }
  wait(0);

  printf("Parent Tracking Value: %d\n", parent_val);
  uptime();
  exit(0);
}
