#ifndef XV6_X86_64_KCLOCK_H
#define XV6_X86_64_KCLOCK_H

// This file comes from JOS.
// ref. https://wiki.osdev.org/Detecting_Memory_(x86)#CMOS

#include "inc/types.h"

#define IO_RTC 0x070 /* RTC port */

#define MC_NVRAM_START 0xe /* start of NVRAM: offset 14 */
#define MC_NVRAM_SIZE 50   /* 50 bytes of NVRAM */

/* NVRAM bytes 7 & 8: base memory size */
#define NVRAM_BASELO (MC_NVRAM_START + 7) /* low byte; RTC off. 0x15 */
#define NVRAM_BASEHI (MC_NVRAM_START + 8) /* high byte; RTC off. 0x16 */

/* NVRAM bytes 9 & 10: extended memory size (between 1MB and 16MB) */
#define NVRAM_EXTLO (MC_NVRAM_START + 9)  /* low byte; RTC off. 0x17 */
#define NVRAM_EXTHI (MC_NVRAM_START + 10) /* high byte; RTC off. 0x18 */

/* NVRAM bytes 38 and 39: extended memory size (between 16MB and 4G) */
#define NVRAM_EXT16LO (MC_NVRAM_START + 38) /* low byte; RTC off. 0x34 */
#define NVRAM_EXT16HI (MC_NVRAM_START + 39) /* high byte; RTC off. 0x35 */

uint mc146818_read(uint reg);
void mc146818_write(uint reg, uint datum);

#endif /* XV6_X86_64_KCLOCK_H */
