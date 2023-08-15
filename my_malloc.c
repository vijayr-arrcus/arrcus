#define _GNU_SOURCE
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static __thread int no_hook_malloc = 0;
//static __thread int no_hook_realloc = 0;
//static __thread int no_hook_calloc = 0;
#define no_hook_realloc no_hook_malloc
#define no_hook_calloc no_hook_malloc
static int trace = 1;
static int trace_fd= -1;

__attribute__((constructor))
static void trace_init (void)
{
  remove("trace.log");
  trace_fd = open("trace.log", O_CREAT|O_WRONLY|O_APPEND, 0666);
}

void*
malloc (size_t size)
{
  time_t tm;
#define SIZE 100
  int nptrs;
  void *buffer[100];
  char *time_buf;
  static void *(*malloc_ptr)(size_t size);
  //printf("inside my malloc \n");
  if (!malloc_ptr) {
    malloc_ptr = dlsym(RTLD_NEXT, "malloc");
  }
  if (no_hook_malloc || !trace || (trace_fd == -1)) {
    return (malloc_ptr(size));
  }
  time(&tm);
  no_hook_malloc = 1;
  time_buf = ctime(&tm);
  time_buf[strlen(time_buf)-1] = '\0';
  dprintf(trace_fd, "\n\t ------------------- \n");
  dprintf(trace_fd, "[%s][malloc]:\n", time_buf);
  nptrs = backtrace(buffer, SIZE);
  backtrace_symbols_fd(buffer, nptrs, trace_fd);
  printf("\n");
  //return NULL;
  no_hook_malloc = 0;
  return malloc_ptr(size);
}
void* calloc(size_t elements, size_t size)
{
  time_t tm;
#define SIZE 100
  int nptrs;
  void *buffer[100];
  char *time_buf;
  static void *(*calloc_ptr)(size_t, size_t) = NULL;
  //printf("inside my malloc \n");
  if (!calloc_ptr) {
    calloc_ptr = (void * (*)())dlsym(RTLD_NEXT, "calloc");
  }
  if (no_hook_calloc || !trace || (trace_fd == -1)) {
    return (calloc_ptr(elements, size));
  }
  time(&tm);
  no_hook_calloc = 1;
  time_buf = ctime(&tm);
  time_buf[strlen(time_buf)-1] = '\0';
  dprintf(trace_fd, "\n\t ------------------- \n");
  dprintf(trace_fd, "[%s][calloc]:\n", time_buf);
  nptrs = backtrace(buffer, SIZE);
  backtrace_symbols_fd(buffer, nptrs, trace_fd);
  printf("\n");
  //return NULL;
  no_hook_calloc = 0;
  return calloc_ptr(elements, size);
}

void *realloc (void *oldptr, size_t size)
{
  time_t tm;
#define SIZE 100
  int nptrs;
  void *buffer[100];
  char *time_buf;
  static void *(*realloc_ptr)(void *, size_t) = NULL;

  if (!realloc_ptr) {
    realloc_ptr = dlsym(RTLD_NEXT, "realloc");
  }

  if (no_hook_realloc || !trace || (trace_fd == -1)) {
    return (realloc_ptr(oldptr, size));
  }
  time(&tm);
  no_hook_realloc = 1;
  time_buf = ctime(&tm);
  time_buf[strlen(time_buf)-1] = '\0';
  dprintf(trace_fd, "\n\t ------------------- \n");
  dprintf(trace_fd, "[%s][realloc]:\n", time_buf);
  nptrs = backtrace(buffer, SIZE);
  backtrace_symbols_fd(buffer, nptrs, trace_fd);
  printf("\n");
  no_hook_realloc = 0;
  return realloc_ptr(oldptr, size);
}
void trace_set (int val)
{
   trace = val;
}

__attribute__((destructor))
int final_func (void)
{
  if (trace_fd != -1) {
    close(trace_fd);
  }
  return 0;
}
