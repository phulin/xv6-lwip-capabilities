#ifndef XV6_LWIP_SYS_ARCH_H_
#define XV6_LWIP_SYS_ARCH_H_

#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)
#define SYS_ARCH_UNPROTECT(lev)

#include "cc.h"
#include "../../include/lwip/err.h"
#include "../../../../thread.h"

struct sem;
typedef struct sem* sys_sem_t;

struct mbox;
typedef struct mbox* sys_mbox_t;

typedef kproc_t sys_thread_t;

void sys_init(void);

//sys_sem_t sys_sem_new(u8_t count);
err_t sys_sem_new(sys_sem_t *sem, u8_t count);
void sys_sem_signal(sys_sem_t *sem);
void sys_sem_free(sys_sem_t *sem);
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout);

err_t sys_mbox_new(sys_mbox_t *mbox, int size);
void sys_mbox_free(sys_mbox_t *mbox);
void sys_mbox_post(sys_mbox_t *mbox, void *msg);
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg);
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout);

#endif // XV6_LWIP_SYS_ARCH_H_
