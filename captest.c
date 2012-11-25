// Create a zombie process that 
// must be reparented at exit.

#include "types.h"
#include "stat.h"
#include "user.h"

int stdout = 1;

int main(void)
{
  uint cmode = 3;

  printf(stdout, "Current Mode: %x\n", cmode);
  cap_getmode(&cmode);
  printf(stdout, "Current Mode: %x\n", cmode);
  cap_enter();
  cap_getmode(&cmode);
  while(1)
    sleep(5);
  printf(stdout, "Current Mode: %x\n", cmode);
  printf(stdout, "Capabilities work!!!\n");
  return 0;
  //exit();
}
