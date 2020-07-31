#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "types.h"
#include "x86.h"

// ref. http://oswiki.osask.jp/?VGA
// ref. https://os.phil-opp.com/vga-text-mode/

// TODO 1: uart
// TODO 2: add useful info to panic

static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void printint(long xx, int base, int sign) {
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  ulong x;

  if (sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);

  if (sign)
    buf[i++] = '-';

  while (--i >= 0)
    consputc(buf[i]);
}

// Print to the console. only understands %d, %x, %p, %s.
// FIXME: Accept up to 4 arguments for now.
void cprintf(char *fmt, ...) {
  int i, c, locking;
  void **argp;
  char *s;

  locking = cons.locking;
  if (locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  void *args[4] = {0, 0, 0, 0};
  __asm__ volatile("mov %%rsi,%0" : "=a"(args[0]) : :);
  __asm__ volatile("mov %%rdx,%0" : "=a"(args[1]) : :);
  __asm__ volatile("mov %%rcx,%0" : "=a"(args[2]) : :);
  __asm__ volatile("mov %%r8,%0" : "=a"(args[3]) : :);
  argp = args;

  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 0)
      break;
    switch (c) {
    case 'd':
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
      // it has to cast as int or accept value sign extended
      printint((long)(int)*argp++, 10, 1);
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
      break;
    case 'x':
    case 'p':
      printint((long)*argp++, 16, 0);
      break;
    case 's':
      if ((s = (char *)*argp++) == 0)
        s = "(null)";
      for (; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if (locking)
    release(&cons.lock);
}

void panic(char *s) {
  // TODO2
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  // cprintf("lapicid %d: panic: ", lapicid());
  cprintf("kernel panic! lapicid %d: panic: %s\n", -1, s);
  // getcallerpcs(&s, pcs);
  // for(i=0; i<10; i++)
  //   cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for (;;)
    ;
}

#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort *)P2V(0xb8000); // CGA memory

static void cgaputc(int c) {
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT + 1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT + 1);

  if (c == '\n')
    pos += 80 - pos % 80;
  else if (c == BACKSPACE) {
    if (pos > 0)
      --pos;
  } else
    crt[pos++] = (c & 0xff) | 0x0700; // black on white

  if (pos < 0 || pos > 25 * 80)
    panic("pos under/overflow");

  if ((pos / 80) >= 24) { // Scroll up.
    memmove(crt, crt + 80, sizeof(crt[0]) * 23 * 80);
    pos -= 80;
    memset(crt + pos, 0, sizeof(crt[0]) * (24 * 80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT + 1, pos >> 8);
  outb(CRTPORT, 15);
  outb(CRTPORT + 1, pos);
  crt[pos] = ' ' | 0x0700;
}

void consputc(int c) {
  if (panicked) {
    cli();
    for (;;)
      ;
  }

  // TODO1
  if (c == BACKSPACE) {
    // uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    // uartputc(c);
    cgaputc(c);
}