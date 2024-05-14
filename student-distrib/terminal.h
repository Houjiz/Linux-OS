 /*terminal's header file*/

#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib.h"
#include "keyboard.h"
#include "paging.h"
#include "syscall.h"
#include "x86_desc.h"
#include "schedule.h"


#define CURSOR_PORT         0x3D4
#define CURSOR_DATA         0x3D5
#define VID_TERM_A          0xB9000
#define VID_TERM_B          0xBA000
#define VID_TERM_C          0xBB000
#define PAGING_TERM_BASE    185
#define MAX_TERMINAL        3
#define MAX_TASK_PER_TERM   4


typedef struct terminal_t
{   
    int32_t term_id;
    int32_t running;
    char keyboard_buf[KEYBOARD_BUF_SIZE];
    //char temp_kb_buf[KEYBOARD_BUF_SIZE];
    int keyboard_buf_length;
    int screen_x;
    int screen_y;
    int enter_flag;
    char* video_mem;
    int task_counter;
    int32_t pid_on_duty;
}terminal_t;

/*ternimal driver functions*/
/* open terminal */
int32_t terminal_open(const uint8_t* filename);
/* close terminal */
int32_t terminal_close(int32_t fd);
/* read from terminal */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
/* write to the terminal from the buffer*/
int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes);
/*clean buffer*/
void buf_cleaner(int32_t term_id);

/*source from https://wiki.osdev.org/Text_Mode_Cursor */
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor(int x, int y);

/*muilti-terminal implement*/
void terminal_init();
void terminal_clear();
void terminal_switch(int32_t new_term_id);

extern int old_length;
extern terminal_t terminal_list[MAX_TERMINAL];
extern int32_t cur_term_id;
extern int32_t term_running;
extern int32_t task_counter;
extern int32_t term_counter;
extern int32_t first_shell_flag;

#endif/*TERMINAL_H*/


