#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//TODO

int
sys_clone(void) 
{
  //set up args
  void(*fcn)(void*, void*);
  void *stack, *arg1, *arg2;
  
  //push args and check for error
  if (argptr(0, (void*)&fcn, sizeof(void*)) < 0)
    return -1;
  if (argptr(1, (void*)&arg1, sizeof(void*)) < 0)
    return -1;
  if (argptr(2, (void*)&arg2, sizeof(void*)) < 0)
    return -1;
  if (argptr(3, (void*)&stack, sizeof(void*)) < 0)
    return -1;
  
  //Check here for page alignment
  if ((uint)stack % PGSIZE != 0 || (uint)stack + PGSIZE > proc->sz)
    return -1;
  
  return clone(fcn, arg1, arg2, stack);
}

int
sys_join(void)
{
  //set up args
  void **stack;
  //push args to stack and check for error
  if (argptr(0, (void*)&stack, sizeof(void**)) < 0)
    return -1;
  return join(stack);
}
