#ifndef LOG_H
#define LOG_H

#define assert_ptr(ptr) massert(ptr, #ptr, __FILE__, __LINE__)
#define log(num,...)    mlog((const char*) make_line(num,__VA_ARGS__), __FILE__, __LINE__)
#define log_pic(num,...) log_pict((const char*) make_line(num,__VA_ARGS__), __FILE__, __LINE__)

#include "..\debug\debug.h"

enum log_errors {
        ASSERTION  = 0xACCE,
        CLOSED_LOG = 0xBAD106,
};

int massert (const void *ptr, const char *var, const char *file, int line);
int log_init (const char *log_file_name);
int mlog (const char *str, const char *file, int line);
int log_pict (const char *str, const char *file, int line);
char* make_line (int argc, ...);
void fill_null(char *str);
int log_dtor ();

#endif /*LOG_H*/
