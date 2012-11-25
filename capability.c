#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "fcntl.h"

// Sets the current process mode to be CAPABILITY mode.
int sys_cap_enter(void)
{
  proc->mode = MODE_CAP;
  return 0;
}

int sys_cap_getmode(void)
{
  uint* mode;

  if(argptr(0, (void*)&mode, sizeof(*mode)) < 0)
    return -1;

  *mode = proc->mode; 

  return 0;
}

int sys_cap_new(void)
{
  int fd;
  cap_rights_t* tr;
  cap_rights_t rights;

  if(argint(0, &fd) < 0)
    return -1;
  if(argptr(1, (void*)&tr, sizeof(*tr)) < 0)
    return -1;
  rights = *tr;
  if (rights == 0)
    return fd;

  return fd;
}

int sys_cap_getrights(void)
{
  struct file *f;
  cap_rights_t *rights;

  if(argfd(0, 0, &f) < 0)
    return -1;
  if(argptr(1, (void*)&rights, sizeof(*rights)) < 0)
    return -1;
  
  *rights = f->rights;

  return 0;
}
