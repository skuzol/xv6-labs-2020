#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

// User code makes a system call with INT T_SYSCALL.
// System call number in %eax.
// Arguments on the stack, from the user call to the C
// library system call function. The saved user %esp points
// to a saved program counter, and then the first argument.

// Fetch the int at addr from the current process.
int
fetchint(uint64 addr, int *ip)
{
  struct proc *p = myproc();

  if(addr >= p->sz || addr+4 > p->sz)
    return -1;
  *ip = *(uint64*)(addr);
  return 0;
}

// Fetch the uint64 at addr from the current process.
int
fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if(addr >= p->sz || addr+sizeof(uint64) > p->sz)
    return -1;
  *ip = *(uint64*)(addr);
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Doesn't actually copy the string - just sets *pp to point at it.
// Returns length of string, not including nul.
int
fetchstr(uint64 addr, char **pp)
{
  char *s, *ep;
  struct proc *p = myproc();

  if(addr >= p->sz)
    return -1;
  *pp = (char*)addr;
  ep = (char*)p->sz;
  for(s = *pp; s < ep; s++){
    if(*s == 0)
      return s - *pp;
  }
  return -1;
}

static uint64
fetcharg(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->tf->a0;
  case 1:
    return p->tf->a1;
  case 2:
    return p->tf->a2;
  case 3:
    return p->tf->a3;
  case 4:
    return p->tf->a4;
  case 5:
    return p->tf->a5;
  }
  panic("fetcharg");
  return -1;
}

// Fetch the nth 32-bit system call argument.
int
argint(int n, int *ip)
{
  *ip = fetcharg(n);
  return 0;
}

int
argaddr(int n, uint64 *ip)
{
  *ip = fetcharg(n);
  return 0;
}

// Fetch the nth word-sized system call argument as a pointer
// to a block of memory of size bytes.  Check that the pointer
// lies within the process address space.
int
argptr(int n, char **pp, int size)
{
  uint64 i;
  struct proc *p = myproc();
 
  if(argaddr(n, &i) < 0)
    return -1;
  if(size < 0 || (uint)i >= p->sz || (uint)i+size > p->sz)
    return -1;
  *pp = (char*)i;
  return 0;
}

// Fetch the nth word-sized system call argument as a string pointer.
// Check that the pointer is valid and the string is nul-terminated.
// (There is no shared writable memory, so the string can't change
// between this check and being used by the kernel.)
int
argstr(int n, char **pp)
{
  uint64 addr;
  if(argaddr(n, &addr) < 0)
    return -1;
  return fetchstr(addr, pp);
}

extern int sys_chdir(void);
extern int sys_close(void);
extern int sys_dup(void);
extern int sys_exec(void);
extern int sys_exit(void);
extern int sys_fork(void);
extern int sys_fstat(void);
extern int sys_getpid(void);
extern int sys_kill(void);
extern int sys_link(void);
extern int sys_mkdir(void);
extern int sys_mknod(void);
extern int sys_open(void);
extern int sys_pipe(void);
extern int sys_read(void);
extern int sys_sbrk(void);
extern int sys_sleep(void);
extern int sys_unlink(void);
extern int sys_wait(void);
extern int sys_write(void);
extern int sys_uptime(void);

static int (*syscalls[])(void) = {
[SYS_fork]    sys_fork,
[SYS_exit]    sys_exit,
[SYS_wait]    sys_wait,
//[SYS_pipe]    sys_pipe,
//[SYS_read]    sys_read,
//[SYS_kill]    sys_kill,
//[SYS_exec]    sys_exec,
//[SYS_fstat]   sys_fstat,
//[SYS_chdir]   sys_chdir,
//[SYS_dup]     sys_dup,
[SYS_getpid]  sys_getpid,
//[SYS_sbrk]    sys_sbrk,
//[SYS_sleep]   sys_sleep,
//[SYS_uptime]  sys_uptime,
//[SYS_open]    sys_open,
//[SYS_write]   sys_write,
//[SYS_mknod]   sys_mknod,
//[SYS_unlink]  sys_unlink,
//[SYS_link]    sys_link,
//[SYS_mkdir]   sys_mkdir,
//[SYS_close]   sys_close,
};

static void
dosyscall(void)
{
  int num;
  struct proc *p = myproc();

  num = p->tf->a7;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    p->tf->a0 = syscalls[num]();
  } else {
    printf("%d %s: unknown sys call %d\n",
            p->pid, p->name, num);
    p->tf->a0 = -1;
  }
}

void
syscall()
{
    if(myproc()->killed)
      exit();
    dosyscall();
    if(myproc()->killed)
      exit();
    return;
}

