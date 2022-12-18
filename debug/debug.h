#ifndef DEBUG_H
#define DEBUG_H

#include "..\include\config.h"


#ifdef DEBUG

#define $ fprintf(stderr, "I'm here File: %s, Line: %d\n", __FILE__, __LINE__);

#define $c(arg) fprintf(stderr, "Char: %c, File: %s, Line: %d\n", arg, __FILE__, __LINE__);

#define $s(arg) fprintf(stderr, "String: %s, File: %s, Line: %d\n", arg, __FILE__, __LINE__);

#define $d(arg) fprintf(stderr, "Decimal: %d, File: %s, Line: %d\n", arg, __FILE__, __LINE__);
#define $lld(arg) fprintf(stderr, "Size_t: %lld, File: %s, Line: %d\n", arg, __FILE__, __LINE__);

#define $dbl(arg) fprintf(stderr, "Double: %llg, File: %s, Line: %d\n", arg, __FILE__, __LINE__);

#define $p(arg) fprintf(stderr, "Pointer: %p, File: %s, Line: %d\n", arg, __FILE__, __LINE__);

#else
        #define $
        #define $c(arg)
        #define $s(arg)
        #define $d(arg)
        #define $lld(arg)
        #define $dbl(arg)
        #define $p(arg)
#endif /*DEBUG*/

#endif /*DEBUG_H*/
