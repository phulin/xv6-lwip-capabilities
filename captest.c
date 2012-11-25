// Create a zombie process that 
// must be reparented at exit.

#include "types.h"
#include "stat.h"
#include "user.h"

int stdout = 1;

int main(void)
{
  uint cmode = 5;
  int work = 1;

  cap_getmode(&cmode);
  printf(stdout, "Current Mode: %x (Should be 0)\n", cmode);
  if (cmode != 0)
    work = 0;
  cap_enter();
  cap_getmode(&cmode);
  printf(stdout, "Current Mode: %x (Should be 1)\n", cmode);
  if (cmode != 1)
    work = 0;
  
  int pid = fork();

  cap_getmode(&cmode);
  printf(stdout, "PID %x: Current Mode %x (Should be 1)\n", pid, cmode);
  if (cmode != 1)
    work = 0;

  if (pid != 0) {
    // Parent
    wait();
    if (work)
      printf(stdout, "Capabilities work!!!\n");
  }
  exit();
}
