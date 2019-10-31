#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

#define PGSIZE 0x1000

typedef struct {
  uint allocd;
  uint aligned;
} addr;

addr addrs[64];

static inline int fetch_and_add(int* variable, int value) {
  __asm__ volatile("lock; xaddl %0, %1"
    : "+r" (value), "+m" (*variable)
    :
    : "memory"
    );
    return value;
}

char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

//TODO

void lock_init(lock_t *lock)
{
  lock->ticket = 0;
  lock->turn = 0;
}

void lock_acquire(lock_t *lock)
{
  int current = fetch_and_add(&lock->ticket, 1);
  while (lock->turn != current);
}

void lock_release(lock_t *lock)
{
  lock->turn += 1;
}

int
thread_create(void(*fcn) (void*, void*), void *arg1, void *arg2)
{
  int i = 65;
  for (int j = 0; j < 64; j++) {
    if (addrs[j].allocd == 0) {
      i = j;
      break;
    }
  }
  void *stack = malloc((uint) PGSIZE * 2);
  if ((uint)stack == 0) {
    return -1;
  }
  addrs[i].allocd = (uint)stack;
  if ((uint)stack % PGSIZE != 0) {
    stack += (PGSIZE - ((uint)stack % PGSIZE));
  }
  addrs[i].aligned = (uint)stack;
  return clone(fcn, arg1, arg2, stack);
}

int
thread_join()
{
  /*
  void *to_join;
  int ret = join(&to_join);
  for (int i = 0; i < 64; i++) {
    if (to_join == (void*)addrs[i].aligned) {
      free((void*)addrs[i].allocd);
      addrs[i].allocd = addrs[i].aligned= 0;
      break;
    }
  }
  */
  void *to_join;
  int ret = join(&to_join);
  free(to_join);
  return ret;
}


