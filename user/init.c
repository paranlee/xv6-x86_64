#include "user.h"

int main(void) {
  pid_t pid1, pid2;

  pid1 = fork();
  if (pid1 < 0) {
    printf("init: 1st fork failed\n");
    exit();
  }
  if (pid1 == 0) {
    // child
    char *argv[] = {"preemptiontest1", "second arg", NULL};
    exec(argv[0], argv);
    printf("init: failed to exec preemptiontest1\n");
    exit();
  }

  pid2 = fork();
  if (pid2 < 0) {
    printf("init: 2nd fork failed\n");
    exit();
  }
  if (pid2 == 0) {
    // child
    char *argv[] = {"preemptiontest2", NULL};
    exec(argv[0], argv);
    printf("init: failed to exec preemptiontest2\n");
    exit();
  }

  int ok1 = 0;
  int ok2 = 0;
  for (int i = 0; i < 2; i++) {
    pid_t pid = wait();
    if (pid == pid1) {
      ok1 = 1;
    } else if (pid == pid2) {
      ok2 = 1;
    }
  }

  if (ok1 && ok2) {
    printf("init: success\n");
  } else {
    printf("init: fail\n");
  }

  for (;;) {
  }
}
