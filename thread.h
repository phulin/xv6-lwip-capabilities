#ifndef XV6_THREAD_H_
#define XV6_THREAD_H_
typedef struct thread * kproc_t;

#ifndef SYS_TIMEOUTS_DEFINED
#define SYS_TIMEOUTS_DEFINED

struct sys_timeouts {
    struct sys_timeout *next;
};
#endif // SYS_TIMEOUTS_DEFINED

struct thread {
    struct proc *p;
    void *data;
    struct sys_timeouts timeouts;
};

kproc_t kproc_start(void (* proc)(void *arg), 
        void *arg, int prio, void *data, char *name);
void kproc_free(kproc_t thread);

#endif // XV6_THREAD_H_
