#include "sys_arch.h"
#include "../../types.h"
#include "../../defs.h"
#include "../../sem.h"
#include "../../include/lwip/sys.h"
#include "../../spinlock.h"
#include "../../assert.h"
#include "../../param.h"
#include "../../mmu.h"
#include "../../proc.h"
#include "../../thread.h"

#define NULL 0

struct sys_timeouts sys_touts;

sys_sem_t sys_sem_new(u8_t count)
{
    sys_sem_t sem = (sys_sem_t)kmalloc(sem_size());
    if (!sem)
        return SYS_SEM_NULL;
    sem_init(sem, count);
    return sem;
}

void sys_sem_free(sys_sem_t sem)
{
    sem_destroy(sem);
    kmfree((char*)sem, sem_size());
}

void sys_sem_signal(sys_sem_t sem)
{
    sem_post(sem);
}

u32_t sys_arch_sem_wait(sys_sem_t sem, u32_t timeout)
{
    int s = millitime(), p;
    int ret;

    if (timeout == 0)
    {
        sem_wait(sem);
        return 0; // What should I return?...
    }

    ret = sem_timedwait(sem, timeout);
    
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
};

sys_mbox_t sys_mbox_new(void)
{
    sys_mbox_t mbox = (sys_mbox_t)kmalloc(sizeof(struct mbox));
    if (!mbox)
        return SYS_MBOX_NULL;
    initlock(&mbox->lock, "mbox");
    mbox->free = (sem_t *)kmalloc(sem_size());
    mbox->queued = (sem_t *)kmalloc(sem_size());
    sem_init(mbox->free, NSLOTS);
    sem_init(mbox->queued, 0);
    mbox->count = 0;
    mbox->head = -1;
    mbox->next = 0;
    return mbox;
};

void sys_mbox_free(sys_mbox_t mbox)
{
    acquire(&mbox->lock);
    sem_destroy(mbox->free);
    sem_destroy(mbox->queued);
    if (mbox->count != 0)
        cprintf("sys_mbox_free: Warning: mbox not free\n");
    release(&mbox->lock);
    kmfree((char*)mbox, sizeof(struct mbox));
}

void sys_mbox_post(sys_mbox_t mbox, void *msg)
{
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

u32_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **msg, u32_t timeout)
{
    u32_t waited = sys_arch_sem_wait(mbox->queued, timeout);
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
        cprintf("fetch failed!\n");
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

u32_t sys_jiffies(void)
{
    return millitime();
}

sys_thread_t sys_thread_new(void (* thread)(void *arg), void *arg, int prio)
{
    return kproc_start(thread, arg, prio, 0, 0);;
}

struct sys_timeouts *
sys_arch_timeouts(void)
{
    if (cp)
        return &cp->thr->timeouts;
    else
        return &sys_touts;
}
    
    
