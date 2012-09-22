#ifndef UTIL_H_
#define UTIL_H_

void log(const char *fmt, ...);
void log_exit(int code, const char *fmt, ...);
void log_exit_perror(int code, const char *fmt, ...);
void *safemalloc(size_t size, const char *fail);
void saferealloc(void **p, size_t new_size, const char *fail);
char *safestrdup(const char *str, const char *fail);


#endif
