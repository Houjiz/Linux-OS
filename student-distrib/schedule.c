#include "schedule.h"
#include "syscall.h"
#include "idt.h"
#include "x86_desc.h"
#include "rofile.h"
#include "lib.h"
#include "paging.h"
#include "i8259.h"
#include "terminal.h"
#include "rtc.h"

// volatile int terminals[3];
uint32_t current_state;
uint32_t next_state;


/* void pit_init()
 * Inputs: none
 * Return Value: void
 * Function: initializes the PIT*/
void pit_init(){
    int pit_value;
    outb(0x36, 0x43);
    pit_value = 11931;
    outb(pit_value & 0xFF, 0x40);
    //pit_value |= inb(0x40)<<8;
    outb(((pit_value & 0xFF00)>>8), 0x40);
    enable_irq(0);
}

/* void progress checker()
 * Inputs: none
 * Return Value: void
 * Function: checks which state we are currently at adjusts accordingly*/
void progress_checker(){
    int i;
    terminal_t *terminal_run;
    for (i = 0; i < MAX_TERMINAL; i++){
        current_state++;
        if (current_state > 2){
            current_state = 0;
        }
        terminal_run = &terminal_list[current_state];
        if (terminal_run->running == 1){
            break;
        }
    }
}

/* void pit_interrupt_handler()
 * Inputs: none
 * Return Value: void
 * Function: handles the scheduling processes*/
void pit_interrupt_handler(){
    
    if (term_counter == 1){
        send_eoi(0);
        return;
    }
    send_eoi(0);
    //similar to syscall exec
    terminal_t *cur_terminal = &terminal_list[current_state];
    int32_t current_pids = cur_terminal->pid_on_duty;
    pcb_t* pcb = get_current_pcb(current_pids);

    //next program from now on, we increment the current state
    
    term_running = current_state;

    progress_checker();

    terminal_t *new_terminal = &terminal_list[current_state];
    int32_t new_pid = new_terminal->pid_on_duty;
    pcb_t* new_pcb = get_current_pcb(new_pid);
    
    //is current terminal visible?
    if(current_state == cur_term_id) {
        //we ARE mapping to video map
        paging_vidmap();

    } else {
        //we aren't mapping to video map
        page_directory[PROCESS_INDEX + 1].P = 1;
        page_directory[PROCESS_INDEX + 1].US = 1;
        page_directory[PROCESS_INDEX + 1].RW = 1;
        page_directory[PROCESS_INDEX + 1].PS = 0;   //4kb pages for memory
        page_directory[PROCESS_INDEX + 1].ADDR = (((uint32_t) &vidmap_page_table) >> 12);
        vidmap_page_table[0].US = 1;
        vidmap_page_table[0].ADDR = PAGING_TERM_BASE + current_state;
        vidmap_page_table[0].P = 1;
        /*TLB flush reerence from osdev*/
        asm volatile (
            "movl %%cr3, %%eax  \n;"
            "movl %%eax, %%cr3  \n;"
            :
            :
            :"memory", "cc", "%eax"
        );    

    }
    /*need to halt paging and restore parent paging*/
    paging_setup_process(new_pid);

    //clear_paging_vidmap();
    
    //switch to new process.
    tss.esp0 = KERNAL_BASE - STACK_SIZE * new_pid - 4;

    uint32_t temp_esp, temp_ebp;
    
     /*save ebp to the stack*/
    asm volatile (
        "movl %%ebp, %0"
        :"=r" (temp_ebp)
    );  
    pcb->term_ebp = temp_ebp;
    /*save esp to the local variable*/
    asm volatile (
        "movl %%esp, %0"
        :"=r" (temp_esp)
    );
    pcb->term_ebp = temp_ebp;
    // sti();
    /*store the status into eax*/
    asm volatile (    
        "movl %0, %%esp   \n;"
        "movl %1, %%ebp   \n;"   
        :
        : "r" (new_pcb->term_ebp), "r" (new_pcb->term_ebp)
    );
    return;

    
}



/* void schedule_init()
 * Inputs: none
 * Return Value: void
 * Function: initializes the state used for scheduling processes*/
void schedule_init(){

    // int i;
    // for(i=0; i<3; i++){
    //     terminals[i] = 0;
    // }
    current_state = 0;

}

