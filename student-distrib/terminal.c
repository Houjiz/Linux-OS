/*terminal functionality of the kernal*/

#include "terminal.h"
#include "lib.h"
#include "keyboard.h"
int old_length;
/* int32_t terminal_open(const uint8_t* filename)
 * Inputs: filename
 * Return Value: 0 (int)
 * Function: Opens the terminal */
int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/* int32_t terminal_close(int32_t fd)
 * Inputs: fd
 * Return Value: -1 (int)
 * Function: closes the terminal */
int32_t terminal_close(int32_t fd){
    /*check fd value if try to close file descriptor*/
    return -1;
}

/* int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: fd, buf, nbytes
 * Return Value: 0 or -1 (int)
 * Function: reads characters from the keyboard buf and cleans the keyboard buf, outputs the length */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    int32_t index; /*return and loop index*/
    terminal_t * cur_terminal = &terminal_list[cur_term_id];
    char* temp_buf = buf;
    /*boundary check*/
    if (temp_buf== NULL || nbytes < 0 || fd != 0){
        /*fail then return -1*/
        return -1;
    }
    if (nbytes == 0){
        return 0;
    }
    /*critical section begins*/
    treminal_read_flag = 0;
    /*wait until enter is pressed*/
    sti();
    while(cur_terminal->enter_flag == 0);
    //while(enter_flag == 0);
    cli();
    treminal_read_flag = 1;
    /*clear the buf*/
    int i; /*loop index*/
    for (i = 0; i < KEYBOARD_BUF_SIZE; i++){
        temp_buf[i] = '\0';
    }
    /*handle overflow*/
    if (nbytes >= KEYBOARD_BUF_SIZE){
        nbytes = KEYBOARD_BUF_SIZE - 1;
        for (index = 0; index < nbytes; index++){
            /*write into the buffer*/
            temp_buf[index] = cur_terminal->keyboard_buf[index];
        
            /*clear the keyboard buffer*/
            cur_terminal->keyboard_buf[index] = '\0';
            keyboard_buf[index] = '\0';
        }
        /*make sure ends with \n*/
        temp_buf[nbytes] = '\n';
        index += 1;
    } else {
        for (index = 0; index < nbytes; index++){
            /*write into the buffer*/
            temp_buf[index] = cur_terminal->keyboard_buf[index];
        
            /*clear the keyboard buffer*/
            cur_terminal->keyboard_buf[index] = '\0';
            keyboard_buf[index] = '\0';
        }
        /*make sure ends with \n*/
        temp_buf[nbytes] = '\n';
        
    }
    
    old_length = keyboard_buf_length;
    keyboard_buf_length = 0;
    enter_flag = 0;
    cur_terminal->enter_flag = 0;
    
    /*end of critical section*/
    //sti();
    
    return index;
}


/* */

/* int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: fd, buf, nbytes
 * Return Value: 0 or -1 (int)
 * Function: write to the terminal from the buffer*/
int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes){
    int i; /*loop index*/
    char* temp_buf = (char*)buf;
    /*boundary check for invalid input*/
    if (temp_buf== NULL || nbytes < 0 || fd != 1){
        /*fail then return -1*/
        return -1;
    }
    /*critical section begins*/
    cli();

    
    /*iterate the buf and print them to the terminal*/
    for (i = 0; i < nbytes; i++){
        /*check if nbytes larger than buf size*/
        if (temp_buf[i] == NULL){
            continue;
        }
        if (term_running == cur_term_id){
            putc(temp_buf[i]);
        }else{
            terminal_putc(temp_buf[i], term_running);
            //putc(temp_buf[i]);
        }
    }

    /*end of critical section*/
    sti();
    return nbytes;

}


/* void buf_cleaner(int32_t term_id)
 * Inputs: term_id
 * Return Value: void
 * Function: clears the keyboard buffer for different terminals*/
void buf_cleaner(int32_t term_id){
    int i; /*loop index*/
    cli();
    for (i = 0; i < KEYBOARD_BUF_SIZE; i++){
        terminal_list[term_id].keyboard_buf[i] = '\0';
    }
    terminal_list[i].keyboard_buf_length = 0;
    sti();
}

terminal_t terminal_list[MAX_TERMINAL];
int32_t cur_term_id;
int32_t term_running;
int32_t task_counter;
int32_t term_counter;
int32_t first_shell_flag = 0;

/* void terminal_init()
 * Inputs: none
 * Return Value: void
 * Function: initializes the different terminals*/
void terminal_init(){
    //cli();
    int i;
    for (i = 0; i < MAX_TERMINAL; i++){
        /*setup terminal id*/
        terminal_list[i].term_id = i;
        /*setup paging and video memory for each terminal*/
        if (i == 0){
            terminal_list[i].video_mem = (char*) VID_TERM_A;
            page_table[PAGING_TERM_BASE + i].ADDR = VID_TERM_A >> 12;
            page_table[PAGING_TERM_BASE + i].P = 1;
            // /*TLB flush reference from osdev*/
            // asm volatile (
            //     "movl %%cr3, %%eax  \n;"
            //     "movl %%eax, %%cr3  \n;"
            //     :
            //     :
            //     :"memory", "cc", "%eax"
            // );
        }
        if (i == 1){
            terminal_list[i].video_mem = (char*) VID_TERM_B;
            page_table[PAGING_TERM_BASE + i].ADDR = VID_TERM_B >> 12;
            page_table[PAGING_TERM_BASE + i].P = 1;
            // /*TLB flush reference from osdev*/
            // asm volatile (
            //     "movl %%cr3, %%eax  \n;"
            //     "movl %%eax, %%cr3  \n;"
            //     :
            //     :
            //     :"memory", "cc", "%eax"
            // );
        }
        if (i == 2){
            terminal_list[i].video_mem = (char*) VID_TERM_C;
            page_table[PAGING_TERM_BASE + i].ADDR = VID_TERM_C >> 12;
            page_table[PAGING_TERM_BASE + i].P = 1;
            // /*TLB flush reference from osdev*/
            // asm volatile (
            //     "movl %%cr3, %%eax  \n;"
            //     "movl %%eax, %%cr3  \n;"
            //     :
            //     :
            //     :"memory", "cc", "%eax"
            // );
        }
        /*clear video memory for terminal init*/
        clear();
        terminal_list[i].screen_x = 0;
        terminal_list[i].screen_y = 0;
        buf_cleaner(i);
        terminal_list[i].task_counter = 0;
        terminal_list[i].pid_on_duty = -1;
    }
    /*TLB flush reference from osdev*/
            asm volatile (
                "movl %%cr3, %%eax  \n;"
                "movl %%eax, %%cr3  \n;"
                :
                :
                :"memory", "cc", "%eax"
            );
    /*initialize with terminal 0*/
        cur_term_id = 0;
        terminal_list[0].running = 1;
        term_running = 0; 
        task_counter = 0;
        term_counter = 1;
        
        first_shell_flag = 1;


        // screen_x = terminal_list[0].screen_x;
        // screen_y = terminal_list[0].screen_y;
        // keyboard_buf_length = terminal_list[0].keyboard_buf_length;
        // memcpy((void*) keyboard_buf, (const void*) terminal_list[0].keyboard_buf, (uint32_t) KEYBOARD_BUF_SIZE);
        // memcpy((void*) video_mem, (const void*) terminal_list[0].video_mem, (uint32_t) NUMPAGES4KB);
        // update_cursor(screen_x, screen_y + 1);
        //sti();
}

void terminal_clear(){
    return;
}

/* void terminal_switch(int32_t new_term_id)
 * Inputs: new_term_id
 * Return Value: void
 * Function: switching between terminals*/
void terminal_switch(int32_t new_term_id){
    cli();
    int i;
    if (new_term_id == cur_term_id){
        /*if the one is the same one running, do nothing*/
        sti();
        return;
    }
    terminal_t *new_term = &terminal_list[new_term_id];
    terminal_t *old_term = &terminal_list[cur_term_id];
    term_running = new_term_id;
    cur_term_id = new_term_id;
    new_term->term_id = cur_term_id;
    /*save all old terminal's status*/
    old_term->screen_x = screen_x;
    old_term->screen_y = screen_y;
    old_term->keyboard_buf_length = keyboard_buf_length;
    memcpy((void*) old_term->keyboard_buf, (const void*) keyboard_buf, (uint32_t) KEYBOARD_BUF_SIZE);
    memcpy((void*) (old_term->video_mem), (const void*) (VIDEO), (uint32_t) NUMPAGES4KB); 
    for (i = 0; i < KEYBOARD_BUF_SIZE; i++){
        keyboard_buf[i] = '\0';
    }   
    uint32_t current_pid = old_term->pid_on_duty;
    pcb_t* pcb = get_current_pcb(current_pid);
     /*save ebp to the stack*/
    asm volatile (
        "movl %%ebp, %0"
        :"=r" (pcb->ebp)
    );  
    /*save esp to the local variable*/
    asm volatile (
        "movl %%esp, %0"
        :"=r" (pcb->esp)
    );

    /*restore all new terminal's status*/
    screen_x = new_term->screen_x;
    screen_y = new_term->screen_y;
    keyboard_buf_length = new_term->keyboard_buf_length;
    memcpy((void*) keyboard_buf, (const void*) new_term->keyboard_buf, (uint32_t) KEYBOARD_BUF_SIZE);
    memcpy((void*) (VIDEO), (const void*) (new_term->video_mem), (uint32_t) NUMPAGES4KB);
    update_cursor(screen_x, screen_y + 1);
    
    uint32_t new_pid = new_term->pid_on_duty;
    pcb_t* new_pcb = get_current_pcb(new_pid);
    if (new_term->running == 0){
        /*if the new terminal is first time opening*/
        new_term->running = 1;
        first_shell_flag = 1;
        printf("TERMINAL #%d are taking charge\n", new_term_id);
        syscall_execute((uint8_t*) "shell");
    }
    term_counter++;
    //switch to new process.
    tss.esp0 = KERNAL_BASE - STACK_SIZE * new_pid - 4;

    /*need to halt paging and restore parent paging*/
    paging_setup_process(new_pid);

    /*store the status into eax*/
    asm volatile (    
        "movl %0, %%esp   \n;"
        "movl %1, %%ebp   \n;"   
        :
        : "g" (new_pcb->esp), "g" (new_pcb->ebp)
        : "memory", "cc"
    );

    sti();
}

/* void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
 * Inputs: cursor_start, cursor_end
 * Return Value: void
 * Function: enables the cursor */
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x0A, CURSOR_PORT);
	outb( (inb(CURSOR_DATA) & 0xC0) | cursor_start, CURSOR_DATA);

	outb(0x0B, CURSOR_PORT);
	outb((inb(CURSOR_DATA) & 0xE0) | cursor_end, CURSOR_DATA);

}

/* void disable_cursor(void)
 * Inputs: none
 * Return Value: void
 * Function: disables the cursor */
void disable_cursor()
{
	outb(0x0A, CURSOR_PORT);
	outb(0x20, CURSOR_DATA);
}


/* void update_cursor(int x, int y)
 * Inputs: x, y
 * Return Value: void
 * Function: updates the cursor */
void update_cursor(int x, int y)
{
	uint16_t pos = y * NUM_COLS + x;

	outb(0x0F, CURSOR_PORT);
	outb((uint8_t)(pos & 0xFF), CURSOR_DATA);
	outb(0x0E, CURSOR_PORT);
	outb((uint8_t)((pos >> 8) & 0xFF), CURSOR_DATA);
}


