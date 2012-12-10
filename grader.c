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
  char new1[] = "Patrick: B\nSteven: A+\n";
  char new2[] = "Patrick: A+\nSteven: B\n";
  char *new = new1;
  char buf[512];

  fd = open("grades.txt", O_CREATE | O_RDWR);
  if (fd < 0) {
    printf(1, "open failed: %e", fd);
    exit();
  }
  write(fd, old, sizeof(old));
  close(fd);
  fd = open("grades.txt", O_CREATE | O_RDWR);
  if (fd < 0) {
    printf(1, "open failed: %e", fd);
    exit();
  }
  int n = read(fd, buf, sizeof(old));
  buf[n] = '\0';
  printf(1, "grades.txt:\n%s", buf);
  close(fd);

  fd = open("grades.txt", O_CREATE | O_RDWR);
  if (fd < 0) {
    printf(1, "open failed: %e", fd);
    exit();
  }
  cap_enter();
  int pid = fork();
  if (pid == 0) {
    int newfd = cap_new(fd, CAP_STAT | CAP_SEEK);

    close(fd);
    fd = newfd;
    new = new2;
  }

  cap_rights_t rights;
  cap_getrights(fd, &rights);
  printf(1, "\nTrying to write [caps %d]:\n%s", rights, new);
  if (write(fd, new, sizeof(new1)) > 0) {
    printf(1, "Write succeeded on pid %d\n", pid);
  } else {
    printf(1, "Write failed on pid %d\n", pid);
  }

  close(fd);

  if (pid != 0) {
    wait();

    fd = open("grades.txt", O_RDWR);
    if (fd < 0) {
      printf(1, "open failed: %e", fd);
      exit();
    }
    int n = read(fd, buf, sizeof(old));
    buf[n] = '\0';
    printf(1, "\nnew grades.txt:\n%s", buf);
    close(fd);
  }

  exit();
}
