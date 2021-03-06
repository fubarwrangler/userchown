#ifndef UTIL_H_
#define UTIL_H_

void log_msg(const char *fmt, ...);
void log_exit(int code, const char *fmt, ...) __attribute__((noreturn));
void log_exit_perror(int code, const char *fmt, ...) __attribute__((noreturn));
void *safemalloc(size_t size, const char *fail);
void saferealloc(void **p, size_t new_size, const char *fail);
char *safestrdup(const char *str, const char *fail);
void pathsplit(const char *path, char **dir, char **file);
char *pathjoin(const char *dir, const char *file);
char *expand_dir(const char *dir);
bool is_directory(const char *path);
void debug(const char *fmt, ...);


extern int _debug;

#endif
