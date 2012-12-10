#ifndef XV6_ASSERT_H_
#define XV6_ASSERT_H_

#define assert(expr) \
    ((expr) ? (void)0 : __assert(__func__, __FILE__, \
        __LINE__, #expr))

void __assert(const char *, const char *, int, const char *);

#endif // XV6_ASSERT_H_

