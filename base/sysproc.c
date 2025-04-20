#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int ktable[NPROC];
int kctable[NPROC];
EXPORT_SYMBOL(ktable);

int sys_ktable_access(void) {
  int access_type; // 0 for write, 1 for read
  int completion;
  int pid;

  if(argint(0, &access_type) < 0)
    return -1;
  if(argint(1, &completion) < 0)
    return -1;
  if(argint(2, &pid) < 0)
    return -1;

  if (access_type == 0) {
    if (completion == 0) {
      for (int i = 0; i < NPROC; i++) {
        if (ktable[i] == 0) {
          ktable[i] = pid;
          kctable[i] = 0;
          break;
        } 
      }
    } else {
      for (int i = 0; i < NPROC; i++) {
        if (ktable[i] == pid) {
          kctable[i] = 1;
          break;
        }
      }
    }
  } else {
    for (int i = 0; i < NPROC; i++) {
      if (ktable[i] != 0 && kctable[i] == 1) {
        int kpid = ktable[i];
        ktable[i] = 0;
        return kpid;
      }
    }
  }

  return 0;
}

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
sys_killpid(void) {
  int pid;
  if (argint(0, &pid) < 0) return -1;
  return killpid(pid);
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
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
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
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int sys_shutdown(void)
{
  /* Either of the following will work. Does not harm to put them together. */
  outw(0xB004, 0x0|0x2000); // working for old qemu
  outw(0x604, 0x0|0x2000); // working for newer qemu
  
  return 0;
}

int
sys_cps(void)
{
  return cps();
}

int
sys_chpr(void)
{
  int pid, pr;
  if(argint(0, &pid) < 0)
    return -1;
  if(argint(1, &pr) < 0)
    return -1;

  return chpr(pid, pr);
}


