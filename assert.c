#include "assert.h"
#include "types.h"
#include "defs.h"

void __assert(const char *func, const char *file, 
        int line, const char *expr)
{
    cprintf("assert failed: %s at %s:%d, expression %s\n",
            func, file, line, expr);
}

