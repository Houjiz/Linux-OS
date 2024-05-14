#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "syscall.h"
#include "idt.h"
#include "x86_desc.h"
#include "rofile.h"
#include "lib.h"
#include "paging.h"
#include "i8259.h"
#include "terminal.h"
#include "rtc.h"

extern void pit_init();
extern void pit_interrupt_handler();
extern void schedule_init();
void progress_checker();

//which terminal are we cur on? 0-2
extern uint32_t current_state;
//next term, functionally used as "vidmap location". 1-3
extern uint32_t next_state;

#endif
