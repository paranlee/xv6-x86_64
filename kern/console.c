#include "defs.h"
#include "file.h"
#include "memlayout.h"
#include "proc.h"
#include "spinlock.h"
#include "trap.h"
#include "x86.h"

// ref. http://oswiki.osask.jp/?VGA
// ref. https://os.phil-opp.com/vga-text-mode/

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

// Print to the console. only understands %c, %d, %x, %p, %s.
// FIXME: Accept up to 5 arguments for now.
void cprintf(char *fmt, ...) {
  int i, c, locking;
  void **argp;
  char *s;

  // have to do copy them to args at first because these values might be
  // overwrited
  void *args[5] = {0, 0, 0, 0, 0};
  prepare_args(args);
  argp = args;

  locking = cons.locking;
  if (locking) {
    acquire(&cons.lock);
  }

  if (fmt == 0) {
    panic("null fmt");
  }

  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (c != '%') {
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 0)
      break;
    switch (c) {
    case 'c':
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
      consputc((char)*argp++);
#pragma GCC diagnostic pop
      break;
    case 'd':
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
      // it has to cast as int or accept value sign extended
      printint((long)(int)*argp++, 10, 1);
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
  cprintf("kernel panic! lapicid %d: panic: %s\n", lapicid(), s);
  // getcallerpcs(&s, pcs);
  // for(i=0; i<10; i++)
  //   cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for (;;)
    __asm__ volatile("hlt");
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

  if (c == '\n') {
    pos += 80 - pos % 80;
  } else if (c == BACKSPACE) {
    if (pos > 0) {
      --pos;
    }
  } else {
    crt[pos++] = (c & 0xff) | 0x0700; // black on white
  }

  if (pos < 0 || pos > 25 * 80) {
    panic("pos under/overflow");
  }

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

  if (c == BACKSPACE) {
    uartputc('\b');
    uartputc(' ');
    uartputc('\b');
  } else {
    uartputc(c);
  }
  cgaputc(c);
}

#define INPUT_BUF 128

struct {
  char buf[INPUT_BUF];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} input;

#define C(x) ((x) - '@') // Control-x

void consoleintr(int (*getc)(void)) {
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while ((c = getc()) >= 0) {
    switch (c) {
    case C('P'): // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'): // Kill line.
      while (input.e != input.w &&
             input.buf[(input.e - 1) % INPUT_BUF] != '\n') {
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('H'):
    case '\x7f': // Backspace
      if (input.e != input.w) {
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    default:
      if (c != 0 && input.e - input.r < INPUT_BUF) {
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c);
        if (c == '\n' || c == C('D') || input.e == input.r + INPUT_BUF) {
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if (doprocdump) {
    // TODO for procdump
    // procdump(); // now call procdump() wo. cons.lock held
  }
}

int consoleread(struct inode *ip, char *dst, int n) {
  int target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while (n > 0) {
    while (input.r == input.w) {
      if (myproc()->killed) {
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if (c == C('D')) { // EOF
      if (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if (c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int consolewrite(struct inode *ip, char *buf, int n) {
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for (i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void consoleinit(void) {
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}
