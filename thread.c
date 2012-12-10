#include "types.h"
#include "defs.h"
#include "thread.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

#define NULL 0

void thread_wrap(void (* thread)(void *arg), void *arg);

extern struct proc* allocproc(void);
extern int setupproc(struct proc*);
extern struct proc *initproc;

kproc_t kproc_start(void (* proc)(void *arg), 
        void *arg, int prio, void *data, char *name)
{
  cprintf("Trying to create thread for %s for %x and arg %x and exit %x\n", name, proc, arg, exit);
    kproc_t thr = (kproc_t)kalloc();
    if (!thr)
        return NULL;
    thr->p = allocproc();
    struct proc *np = thr->p;
    if (!np)
        return NULL;

    setupproc(thr->p);

    np->thr = thr;
    np->parent = initproc;
    np->sz = 0;
    np->chan = 0;
    np->killed = 0;
    thr->data = data;
    memset(np->context, 0, sizeof(np->context));
    thr->timeouts.next = 0;
    if (name == 0)
        safestrcpy(np->name,"[kernel thread]",sizeof(np->name));
    else
        safestrcpy(np->name, name, sizeof(np->name));
    cprintf("Trying to move %x to context\n", (uint)thread_wrap);
    char *sp;
    sp = np->kstack + KSTACKSIZE - 1;

    sp -= sizeof np->tf;
    np->tf = (struct trapframe*) sp;
    sp -= 4;
    *(uint*)sp = (uint) exit;
    sp -= 4;
    *(uint*)sp = (uint) proc;
    sp -= 4;
    *(uint*)sp = (uint) arg;
    sp -= sizeof *np->context;
    np->context = (struct context*) sp;

    (np->context)->eip = (uint)thread_wrap;

    np->cwd = namei("/");
    np->state = RUNNABLE;
  cprintf("Created: Trying to create thread for %s", name);
    return thr;
}

void kproc_free(kproc_t thread)
{
    struct proc *p = thread->p;
    p->thr = 0;
    kfree((char*)thread);
}

void thread_wrap(void (* thread)(void *arg), void *arg)
{
  cprintf("Jumping from wrap to thread %x\n", thread);
    release(&ptablelock);
    thread(arg);
}


