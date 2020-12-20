#ifndef XV6_X86_64_FILE_H
#define XV6_X86_64_FILE_H

#include "fs.h"
#include "inc/types.h"
#include "sleeplock.h"

struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_SOCKET } type;
  int ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe;
  struct inode *ip;
  struct socket *sock;
  uint off;
};

// in-memory copy of an inode
struct inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  int ref;               // Reference count
  struct sleeplock lock; // protects everything below here
  int valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// table mapping major device number to
// device functions
struct devsw {
  int (*read)(struct inode *, char *, int);
  int (*write)(struct inode *, char *, int);
  int (*close)(struct inode *);
};

extern struct devsw devsw[];

#define CONSOLE 1

#endif /* ifndef XV6_X86_64_FILE_H */
