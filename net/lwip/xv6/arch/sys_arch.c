#include "sys_arch.h"
#include "../../../../types.h"
#include "../../../../defs.h"
#include "../../include/lwip/sys.h"
#include "../../include/lwip/err.h"
#include "../../../../assert.h"
#include "../../../../spinlock.h"
#include "../../../../param.h"
#include "../../../../mmu.h"
#include "../../../../proc.h"

#define NULL 0

struct sem {
    struct spinlock lock;
    int val;
    int waiters;
    int valid;
  int name;
};

static int next_name = 2;

void sys_init() {
}

int sem_destroy(sys_sem_t sem)
{
  //cprintf("sem_destroy %x\n", sem->name);
    assert(sem->waiters == 0);
    return 0;
}


void sem_post(sys_sem_t sem)
{
  //cprintf("sem_post %x - %d\n", sem->name, sem->val);
    acquire(&sem->lock);
    sem->val++;
    if ((sem->waiters) && (sem->val > 0))
    {
        wakeup_one(sem); // XXX maybe wakeup?
    }
    release(&sem->lock);
}

void sem_wait(sys_sem_t sem)
{
  //cprintf("sem_wait %x - %d\n", sem->name, sem->val);
    acquire(&sem->lock);
    while (sem->val == 0)
    {
        sem->waiters++;
        sleep(sem, &sem->lock);
        sem->waiters--;
    }
    sem->val--;
    release(&sem->lock);
}

int sem_timedwait(sys_sem_t sem, int timo)
{
  //cprintf("sem_timedwait %x\n", sem->name);
    int ret;

    acquire(&sem->lock);
    for (ret = 0; sem->val == 0 && ret == 0;)
    {
        sem->waiters++;
        ret = msleep_spin(sem, &sem->lock, timo);
        sem->waiters--;
    }
    if (sem->val > 0)
    {
        sem->val--;
        ret = 0;
    }
    release(&sem->lock);

    return ret;
}

int sem_trywait(sys_sem_t *sem)
{
  //cprintf("sem_trywait %x\n", (*sem)->name);
    int ret;

    acquire(&(*sem)->lock);
    if ((*sem)->val > 0)
    {
      (*sem)->val--;
        ret = 1;
    } else {
        ret = 0;
    }
    release(&(*sem)->lock);
    return ret;
}

int sem_value(sys_sem_t *sem)
{
  //cprintf("sem_value %x\n", sem);
    int ret;

    acquire(&(*sem)->lock);
    ret = (*sem)->val;
    release(&(*sem)->lock);
    return ret;
}

int sem_size()
{
    return sizeof(struct sem);
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
  //cprintf("sys_sem_new %x - %d\n", next_name, count);
  *sem = (sys_sem_t) kalloc();//malloc(sem_size());
  assert(count >= 0);
  initlock(&(*sem)->lock, "sem lock");
  (*sem)->val = count;
  (*sem)->valid = 1;
  (*sem)->waiters = 0;
  (*sem)->name = next_name++;
  //cprintf("exit sys_sem_new %x - %d\n", (*sem)->name, (*sem)->val);
  return 0;
}

void sys_sem_free(sys_sem_t *sem)
{
  if (sem) {
    //cprintf("sys_sem_free %x\n", (*sem)->name);
    sem_destroy(*sem);
  }
}

void sys_sem_signal(sys_sem_t *sem)
{
  if (sem) {
    //cprintf("sys_sem_signal %x\n", (*sem)->name);
    sem_post(*sem);
  }
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  //cprintf("sys_arch_sem_wait %x for %x\n", (*sem)->name, timeout);
  if (!sem)
    return -1;

  int s = millitime(), p;
  int ret;

  if (timeout == 0)
  {
    sem_wait(*sem);
    return 0; // What should I return?...
  }
  
  ret = sem_timedwait(*sem, timeout);
  
  p = millitime() - s;
  if (ret == 0)
    return p;
  else
    return SYS_ARCH_TIMEOUT;
}

#define NSLOTS 128

struct mbox {
    struct spinlock lock;
    sys_sem_t free, queued;
    int count, head, next;
    void *slots[NSLOTS];
  int valid;
};

err_t sys_mbox_new(sys_mbox_t *rmbox, int size)
{
  //cprintf("sys_mbox_new\n");
  sys_mbox_t mbox = (sys_mbox_t) kalloc();// malloc(sem_size());
  if (!mbox)
    return -2;
  initlock(&mbox->lock, "mbox");
  mbox->free = (sys_sem_t) kalloc();//malloc(sem_size());
  mbox->queued = (sys_sem_t) kalloc();//malloc(sem_size());
  sys_sem_new(&mbox->free, NSLOTS);
  sys_sem_new(&mbox->queued, 0);
  mbox->count = 0;
  mbox->valid = 1;
  mbox->head = -1;
  mbox->next = 0;
  if (rmbox)
    *rmbox = mbox;

  return 0;
};

void sys_mbox_free(sys_mbox_t *rmbox)
{
  //cprintf("sys_mbox_free\n");
  if (!rmbox)
    return;
  sys_mbox_t mbox = *rmbox;
  acquire(&mbox->lock);
    sem_destroy(mbox->free);
    sem_destroy(mbox->queued);
    if (mbox->count != 0) {
      //cprintf("sys_mbox_free: Warning: mbox not free\n");
    }
    release(&mbox->lock);
}

void sys_mbox_post(sys_mbox_t *rmbox, void *msg)
{
  //cprintf("sys_mbox_post\n");
  if (!rmbox)
    return;
  sys_mbox_t mbox = *rmbox;
    sem_wait(mbox->free);
    acquire(&mbox->lock);
    if (mbox->count == NSLOTS)
    {
        release(&mbox->lock);
        return;
    }
    int slot = mbox->next;
    mbox->next = (slot + 1) % NSLOTS;
    mbox->slots[slot] = msg;
    mbox->count++;
    if (mbox->head == -1)
        mbox->head = slot;

    sem_post(mbox->queued);
    release(&mbox->lock);
}

err_t sys_mbox_trypost(sys_mbox_t *rmbox, void *msg)
{
  //cprintf("sys_mbox_trypost\n");
  if (!rmbox)
    return -2;
  sys_mbox_t mbox = *rmbox;
    acquire(&mbox->lock);
    if (mbox->count == NSLOTS)
    {
        release(&mbox->lock);
        return -1;
    }
    int slot = mbox->next;
    mbox->next = (slot + 1) % NSLOTS;
    mbox->slots[slot] = msg;
    mbox->count++;
    if (mbox->head == -1)
        mbox->head = slot;

    sem_post(mbox->queued);
    release(&mbox->lock);
    return 0;
}



u32_t sys_arch_mbox_fetch(sys_mbox_t *rmbox, void **msg, u32_t timeout)
{
  //cprintf("sys_mbox_fetch\n");
  if (!rmbox)
    return SYS_ARCH_TIMEOUT;
  sys_mbox_t mbox = *rmbox;
    u32_t waited = sys_arch_sem_wait(&mbox->queued, timeout);
    acquire(&mbox->lock);
    if (waited == SYS_ARCH_TIMEOUT)
    {
        release(&mbox->lock);
        return waited;
    }

    int slot = mbox->head;
    if (slot == -1)
    {
        release(&mbox->lock);
        //cprintf("fetch failed!\n");
        return SYS_ARCH_TIMEOUT; // XXX panic is not good...
    }

    if (msg)
        *msg = mbox->slots[slot];

    mbox->head = (slot + 1) % NSLOTS;
    mbox->count--;
    if (mbox->count == 0)
        mbox->head = -1;

    sem_post(mbox->free);
    release(&mbox->lock);
    return waited;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *rmbox, void **msg) {
  if (!rmbox)
    return SYS_ARCH_TIMEOUT;
  sys_mbox_t mbox = *rmbox;
  acquire(&mbox->lock);
  
  int slot = mbox->head;
  if (slot == -1)
  {
        release(&mbox->lock);
        //cprintf("fetch failed!\n");
        return SYS_ARCH_TIMEOUT; // XXX panic is not good...
  }
  
  if (msg)
    *msg = mbox->slots[slot];
  
  mbox->head = (slot + 1) % NSLOTS;
  mbox->count--;
  if (mbox->count == 0)
    mbox->head = -1;
  
  sem_post(mbox->free);
  release(&mbox->lock);
  return SYS_ARCH_TIMEOUT;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio) {
  sys_thread_t thr = kproc_start(thread, arg, prio, 0, 0);
  return thr;
}

int sys_sem_valid(sys_sem_t *sem) {
  if (sem && *sem)
    return (*sem)->valid;
  return 0;
}

void sys_sem_set_invalid(sys_sem_t *sem) {
  if (sem && *sem)
    (*sem)->valid = 0;
}

int sys_mbox_valid(sys_mbox_t *mbox) {
  if (mbox && *mbox)
    return (*mbox)->valid;
  return 0;
}

void sys_mbox_set_invalid(sys_mbox_t *mbox) {
  if (mbox && *mbox)
    (*mbox)->valid = 0;
}

