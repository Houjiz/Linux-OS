#ifndef _rtc_h
#define _rtc_h

#include "types.h"

extern void rtc_init();
extern void rtc_interrupt_handler();
extern void test_interrupts(void);
extern int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes);
extern int rtc_open();
extern int rtc_close();
extern int rtc_read();

#endif

