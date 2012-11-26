#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "fcntl.h"
#include "capability.h"

// Sets the current process mode to be MODE_CAP mode.
int sys_cap_enter(void)
{
  int i;

  proc->mode = MODE_CAP;
  for (i = 0; i < NOFILE; i += 1)
    if (proc->ofile[i])
      proc->ofile[i]->rights = CAP_ALL;
  return 0;
}

// Returns the current process mode in arg0
int sys_cap_getmode(void)
{
  uint* mode;

  if(argptr(0, (void*)&mode, sizeof(*mode)) < 0)
    return -1;

  *mode = proc->mode; 

  return 0;
}

// Creates a new file descriptor with limited capabilities.
int sys_cap_new(void)
{
  int fd;
  struct file* file;
  struct file* newfile;
  cap_rights_t rights;

  if(argfd(0, &fd, &file) < 0)
    return -1;
  if(argint(1, (int*)&rights) < 0)
    return -1;

  // Checks that 'rights' is a subset of 'file->rights'
  if ((~(file->rights) & rights))
    return -1;

  newfile = filealloc();
  newfile->type = file->type;
  newfile->ref = 1;
  newfile->readable = file->readable;
  newfile->writable = file->writable;
  newfile->rights = rights;
  newfile->pipe = file->pipe;
  newfile->ip = file->ip;
  newfile->off = file->off;

  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd] == 0){
      proc->ofile[fd] = newfile;
      return fd;
    }
  }
  return -1;
}

// Returns the capabilities on the file descriptor.
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
