#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "lib.h"
#include "rtc.h"
#include "rofile.h"
#include "keyboard.h"
#include "types.h"
#include "x86_desc.h"
#include "paging.h"
#include "terminal.h"

#define FILE_ARRAY_SIZE     8
#define MAX_NUM_PROG        6
/*user space memory start and end from APPENDIX*/
#define USER_START          0x08048000 
#define USER_END            0x08400000 
#define MB_128              0x08000000
#define MB_132              0x08400000
#define MB_136              0x08800000


/*8MB*/
#define KERNAL_BASE         0x800000  
/*8KB*/
#define STACK_SIZE          0x2000
/*4MB*/
#define PROCESS_SIZE        0x400000
/*Program Location is Always 128MB, each index is 4MB. Program index is therefore 128/4=32*/
#define PROCESS_INDEX       32

#define ARGLEN              128
#define CMDLEN              32

typedef struct file_optable_t
{
    int32_t (*read)(int32_t, void*, int32_t);
    int32_t (*write)(int32_t, void*, int32_t);
    int32_t (*open)(const uint8_t*);
    int32_t (*close)(int32_t);
}file_optable_t;


typedef struct file_entry_t
{
    file_optable_t* optable_ptr;
    int32_t inode;
    int32_t file_pos;
    int32_t flag;
}file_entry_t;

typedef struct pcb_t
{
    /*process indentify*/
    int32_t pid;
    int32_t parent_pcb_pid;
    int32_t terminal_id;
    uint32_t ebp;
    uint32_t esp;
    file_entry_t file_array[FILE_ARRAY_SIZE];
    uint8_t args[ARGLEN];
    uint32_t term_ebp;
    uint32_t term_esp;
}pcb_t;


extern int32_t syscall_execute(const uint8_t * usercall);
extern int32_t syscall_halt(uint8_t status);
extern int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t syscall_write(int32_t fd, void* buf, int32_t nbytes);
extern int32_t syscall_open(const uint8_t* filename);
extern int32_t syscall_close(int32_t fd);
/*for later use*/
extern int32_t syscall_getargs(uint8_t* buf, int32_t nbytes);
extern int32_t syscall_vidmap(uint8_t** screen_start);
extern int32_t syscall_handler(int32_t signum, void* handler_address);
extern int32_t syscall_sigreturn(void);

/* get the current pcb */
extern pcb_t* get_current_pcb(int32_t current_pid);
/*find available fd and change the flag of that entry*/
extern int32_t find_fd(pcb_t *current_pcb);
/*initialize optable*/
extern void optable_init();
/*creating pcb*/
pcb_t* create_pcb();
/*process paging*/
extern void paging_setup_process(int32_t pid);
/*process paging*/
extern void paging_vidmap();
/*clear the vidmap*/
extern void clear_paging_vidmap();

extern int32_t current_pid;
#endif



