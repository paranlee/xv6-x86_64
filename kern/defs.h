#ifndef XV6_X86_64_DEFS_H
#define XV6_X86_64_DEFS_H

#include "types.h"

struct buf;
struct context;
struct cpu;
struct ioapic;
struct proc;
struct sleeplock;
struct spinlock;
struct superblock;

// args.c
void prepare_args(void *args[]);

// bio.c
void binit(void);
struct buf *bread(uint dev, uint blockno);
void bwrite(struct buf *b);
void brelse(struct buf *b);

// console.c
void consoleinit(void);
void consoleintr(int (*)(void));
void cprintf(char *, ...);
void panic(char *) __attribute__((noreturn));

// exec.c
int exec(char *, char **);

// fs.c
void readsb(int dev, struct superblock *sb);

// ide.c
void ideinit(void);
void ideintr(void);
void iderw(struct buf *b);

// ioapic.c
extern volatile struct ioapic *ioapic;
extern uint8_t ioapicid;
void ioapicenable(int irq, int cpunum);
void ioapicinit(void);

// kbd.c
void kbdintr(void);

// lapic.c
extern volatile uint32_t *lapic;
void lapiceoi(void);
int lapicid(void);
void lapicinit(void);
void microdelay(int us);

// log.c
void initlog(int dev);
void log_write(struct buf *);
void begin_op();
void end_op();

// kalloc.c
char *kalloc(void);
void kfree(char *);
void kinit1(void *vstart);
void kinit2();

// mp.c
void mpinit(void);

// picirq.c
void picinit(void);

// proc.c

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
void exit(void);
#pragma GCC diagnostic pop
pid_t fork(void);
struct cpu *mycpu(void);
int cpuid(void);
struct proc *myproc(void);
void sched(void);
void scheduler(void) __attribute__((noreturn));
void sleep(void *chan, struct spinlock *lk);
void userinit(void);
pid_t wait(void);
void wakeup(void *chan);
void yield(void);

// sleeplock.c
void acquiresleep(struct sleeplock *);
void releasesleep(struct sleeplock *);
int holdingsleep(struct sleeplock *);
void initsleeplock(struct sleeplock *, char *);

// swtch.S
void swtch(struct context **, struct context *);

// spinlock.c
void acquire(struct spinlock *);
// void            getcallerpcs(void*, uint*);
int holding(struct spinlock *);
void initlock(struct spinlock *, char *);
void release(struct spinlock *);
void pushcli(void);
void popcli(void);

// string.c
int memcmp(const void *v1, const void *v2, size_t n);
void *memset(void *, int, size_t);
void *memmove(void *, const void *, size_t);
char *safestrcpy(char *s, const char *t, int n);
size_t strlen(const char *s);
int strncmp(const char *p, const char *q, size_t n);

// syscall.c
int arg(int n, uint64_t *ip);
int argstr(int n, char **pp);
int fetchint(uintptr_t addr, uint64_t *ip);
int fetchstr(uintptr_t addr, char **pp);
void syscall(void);

// trap.c
void idtinit(void);
void tvinit(void);

// uart.c
void uartinit(void);
void uartintr(void);
void uartputc(int);

// vm.c
int allocuvm(pte_t *pgdir, size_t oldsz, size_t newsz);
void clearpteu(pte_t *pgdir, char *uva);
int copyout(pte_t *pgdir, uintptr_t va, void *p, size_t len);
pte_t *copyuvm(pte_t *, size_t);
int deallocuvm(pte_t *pgdir, size_t oldsz, size_t newsz);
void freevm(pte_t *pgdir, uintptr_t utop);
void inituvm(pte_t *pgdir, char *init, size_t sz);
void kvmalloc(void);
int loaduvm(pte_t *pgdir, char *addr, char *p_elf, size_t offset, size_t sz);
void seginit(void);
pte_t *setupkvm(void);
void switchkvm(void);
void switchuvm(struct proc *p);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

#endif /* ifndef XV6_X86_64_DEFS_H */
