#ifndef IDT_H
#define IDT_H

#include "x86_desc.h"
#include "rtc.h"
#include "keyboard.h"
#include "interrupts.h"
#include "syscall.h"
// location in the IDT for exception entries
#define DIVERROR    0x00
#define DEBUGEX     0x01
#define NMI         0x02
#define BREAKPT     0x03
#define OVERFLOW    0x04
#define BOUND       0x05
#define INVOP       0x06
#define DEVNOTAVAIL 0x07
#define DOUBLFAULT  0x08
#define COPSEGO     0x09
#define INVALTSS    0x0A
#define SEGNOTP     0x0B
#define STACKFAULT  0x0C
#define GENPROT     0x0D
#define PAGEFAULT   0x0E
//lmao we skip 15 for some reason?
#define FLOATPOINT  0x10
#define ALIGNCHECK  0x11
#define MACHCHECK   0x12
#define SIMDFLOAT   0x13
// location in the IDT for keyboard and RTC entries
#define KEYLOCAL    0x21
#define RTCLOCAL    0x28
#define SYSCALLLOCAL 0x80
#define PITLOCAL    0x20

/*  Handler Functions   */
extern void divideerror();
extern void debug();
extern void nmi_interrupt();
extern void breakpoint();
extern void overflow();
extern void boundrange();
extern void invalopcode();
extern void devnotavail();
extern void doublefault();
extern void coprocessseg();
extern void invaltss();
extern void segnotpres();
extern void stackfault();
extern void genprotection();
extern void pagefault();
extern void fpufloatpoint();
extern void alligncheck();
extern void machinecheck();
extern void simdfloatpoint();
extern void sysCallHandler();

extern void setup_idt();

#endif /*IDT_H*/



