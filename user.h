#include "types.h"

struct stat;
struct fdlist;
struct sockaddr;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(char*, int);
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int cap_enter(void);
int cap_getmode(uint* modep);
int cap_new(int fd, cap_rights_t rights);
int cap_getrights(int fd, cap_rights_t* rights);
int createat(int, char*);
int ioctl(int, int, void*);
int forkwithfds(struct fdlist*);
struct fdlist *getparentfds(void);
int openat(int dfd, char *path, int mode);
int fexec(int, char**);

#ifndef socklen_t
#define socklen_t int
#endif
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int bind(int s, struct sockaddr *name, socklen_t namelen);
int shutdown(int s, int how);
int getpeername (int s, struct sockaddr *name, socklen_t *namelen);
int getsockname (int s, struct sockaddr *name, socklen_t *namelen);
int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
int setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
int sockclose(int s);
int connect(int s, struct sockaddr *name, socklen_t namelen);
int listen(int s, int backlog);
int recv(int s, void *mem, int len, unsigned int flags);
//int read(int s, void *mem, int len);
int recvfrom(int s, void *mem, int len, unsigned int flags,
      struct sockaddr *from, socklen_t *fromlen);
int send(int s, void *dataptr, int size, unsigned int flags);
int sendto(int s, void *dataptr, int size, unsigned int flags,
    struct sockaddr *to, socklen_t tolen);
int socket(int domain, int type, int protocol);

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, char*, ...);
char* fgets(int, char*, int max);
char* gets(char*, int max);
uint strlen(char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
