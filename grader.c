#include "param.h"
#include "types.h"
#include "stat.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "capability.h"
#include "user.h"

int main(void)
{
  int fd;
  char old[] = "Patrick: A\nSteven: A\n";
  char new[] = "Patrick: B\nSteven: A+\n";

  fd = open("grades.txt", O_CREATE | O_RDWR);
  if (fd < 0) {
    printf(1, "open failed: %e", fd);
    exit();
  }
  write(fd, old, sizeof(old));

  cap_enter();
  int pid = fork();
  if (pid == 0) {
    int newfd = cap_new(fd, CAP_STAT | CAP_SEEK);

    close(fd);
    fd = newfd;
  }

  if (write(fd, new, sizeof(new)) > 0) {
    printf(1, "Write succeeded on pid %d\n", pid);
  } else {
    printf(1, "Write failed on pid %d\n", pid);
  }

  close(fd);

  if (pid != 0)
    wait();

  exit();
}
