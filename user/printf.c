#include "user.h"

static void printchar(int fd, char c, int *print_cnt) {
  write(fd, &c, 1);
  (*print_cnt)++;
}

static void printint(int fd, long xx, int base, int sign, int *print_cnt) {
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  unsigned long x;

  neg = 0;
  if (sign && xx < 0) {
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);
  if (neg)
    buf[i++] = '-';

  while (--i >= 0) {
    printchar(fd, buf[i], print_cnt);
  }
}

// The size of args must be equal to or larger than 5.
__inline__ void prepare_args(void *args[]) {
  __asm__ volatile("mov %%rsi,%0" : "=a"(args[0]) : :);
  __asm__ volatile("mov %%rdx,%0" : "=a"(args[1]) : :);
  __asm__ volatile("mov %%rcx,%0" : "=a"(args[2]) : :);
  __asm__ volatile("mov %%r8,%0" : "=a"(args[3]) : :);
  __asm__ volatile("mov %%r9,%0" : "=a"(args[4]) : :);
}

int __attribute__((noinline)) do_printf(const char *fmt, void *args[], int fd) {
  char *s;
  int c, i, state;
  long *ap;
  int print_cnt = 0;

  ap = (long *)args;

  state = 0;
  for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
    if (state == 0) {
      if (c == '%') {
        state = '%';
      } else {
        printchar(fd, c, &print_cnt);
      }
    } else if (state == '%') {
      if (c == 'd') {
        printint(fd, *ap, 10, 1, &print_cnt);
        ap++;
      } else if (c == 'x' || c == 'p') {
        printint(fd, *ap, 16, 0, &print_cnt);
        ap++;
      } else if (c == 's') {
        s = (char *)*ap;
        ap++;
        if (s == 0)
          s = "(null)";
        while (*s != 0) {
          printchar(fd, *s, &print_cnt);
          s++;
        }
      } else if (c == 'c') {
        printchar(fd, *ap, &print_cnt);
        ap++;
      } else if (c == '%') {
        printchar(fd, c, &print_cnt);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        printchar(fd, '%', &print_cnt);
        printchar(fd, c, &print_cnt);
      }
      state = 0;
    }
  }

  return print_cnt;
}

// Print to the stdout. Only understands %c, %d, %x, %p, %s.
// FIXME: the current implementation accepts only five arguments other than fmt.
int printf(const char *fmt, ...) {
  void *args[5] = {0, 0, 0, 0, 0};
  prepare_args(args);
  return do_printf(fmt, args, 0);
}

// Print to the given fd. Only understands %c, %d, %x, %p, %s.
// FIXME: the current implementation accepts only five arguments other than fmt.
int dprintf(int fd, const char *fmt, ...) {
  void *args[5] = {0, 0, 0, 0, 0};
  prepare_args(args);
  return do_printf(fmt, args, fd);
}
