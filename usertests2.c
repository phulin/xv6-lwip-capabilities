#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int stdout = 1;

void
openattest(void)
{
  int dfd, fd;

  dfd = open(".", 0);
  if(dfd < 0){
    printf(stdout, "open cwd failed!\n");
    exit();
  }

  printf(stdout, "openat test\n");
  fd = openat(dfd, "echo", 0);
  if(fd < 0){
    printf(stdout, "openat echo failed!\n");
    exit();
  }
  close(fd);
  fd = openat(dfd, "doesnotexist", 0);
  if(fd >= 0){
    printf(stdout, "openat doesnotexist succeeded!\n");
    exit();
  }

  close(dfd);
  printf(stdout, "open test ok\n");
}

int
main(void)
{
  openattest();

  return 0;
}
