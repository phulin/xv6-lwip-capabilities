// Create a zombie process that 
// must be reparented at exit.

#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "capability.h"

int stdout = 1;

int main(void)
{
  int rootfd, fd, fd2;
  uint cmode = 5;
  int work = 1;


  mkdir("captestdir");
  chdir("captestdir");
  rootfd = open("/captestdir", O_RDONLY);
  fd = open("captest1", O_CREATE | O_RDWR);
  fd2 = open("captest2", O_CREATE | O_RDWR);
  printf(stdout, "FD2: %d\n", fd2);

  cap_getmode(&cmode);
  printf(stdout, "Current Mode: %x (Should be 0)\n", cmode);
  if (cmode != 0)
    work = 0;
  cap_enter();
  cap_getmode(&cmode);
  printf(stdout, "Current Mode: %x (Should be 1)\n", cmode);

  if (cmode != 1)
    work = 0;
  write(fd, "aaaaaaaa", 8);
  //write(fd2, "bbbbbbbb", 8);
  cap_rights_t rights;
  cap_getrights(fd2, &rights);
  printf(stdout, "Rights: %d\n", rights);
  write(fd2, "bbbbbbbb", 8);

  fd2 = cap_new(fd2, CAP_STAT | CAP_SEEK | CAP_WRITE);
  cap_getrights(fd2, &rights);
  printf(stdout, "Rights: %d\n", rights);
  printf(stdout, "FD2: %d\n", fd2);


  write(fd2, "cccccccc", 8);

  close(fd);
  close(fd2);

  printf(stdout, "Root FD: %d\n", rootfd);

  cap_new(rootfd, 0);
  close(rootfd);

  createat(rootfd, "captest4");

  fd = open("captest4", O_RDWR);
  write(fd, "Here is this!", 13);
  printf(stdout, "Here with FD %d\n", fd);
  close(fd);

  fd = open("/captest4", O_RDONLY);
  char* buf;
  read(fd, &buf, 12);
  printf(stdout, buf);

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
