#include "syscall.h"
#include "idt.h"
#include "x86_desc.h"
#include "rofile.h"


/*initialize process id*/
//int32_t current_pid = -1;
int32_t current_pid = 0;
int pid_list[MAX_NUM_PROG] = {0,0,0,0,0,0};

file_optable_t terminal_optable;
file_optable_t rtc_optable;
file_optable_t file_optable;
file_optable_t dir_optable;


/* void syscall_execute(const uint8_t * commands)
 * Inputs: void
 * Return Value: int32_t
 * Function: executes system calls */
int32_t syscall_execute(const uint8_t * commands){
    cli();
    dir_entry_t file_dentry;
    uint8_t file_buf[4];
    //uint8_t buf[40];
    uint8_t* rtn_val;
    pcb_t* pcb;
    terminal_t *cur_terminal = &terminal_list[cur_term_id];
    //terminal_t *running_terminal = &terminal_list[term_running];
    int i;
  

    /*Copy the command Into a tamp buffer 
     Making a new buffer with 1 more large Than the index of previous index
     Load the tamp buffer into new buffer
     Initialize the arg buffer in the global variable with length of 1024
     Load the rest of the args into arg buffer and memcpy to the arg in side of the pcb construct */

    uint8_t parsed_arg[ARGLEN];
    uint8_t parsed_temped[ARGLEN];
    int cmd_index = 0;
    int args_index = 0;
    while (commands[cmd_index] != ' '  && commands[cmd_index] != '\0'){
        parsed_temped[cmd_index] =commands[cmd_index];
        cmd_index++;
    }
    int temp00 = cmd_index + 1;
    uint8_t parsed_cmd[temp00];
    while (args_index < cmd_index){
        parsed_cmd[args_index] = parsed_temped[args_index];
        args_index++;
    }
    parsed_cmd[cmd_index] = '\0';
    cmd_index++;
    args_index = 0;
    for (i = 0; i < ARGLEN; i++){
        parsed_arg[i] = '\0';
    }
    while (commands[cmd_index] != ' ' && commands[cmd_index] != '\0'){
        parsed_arg[args_index] = commands[cmd_index];
        cmd_index++;
        args_index++;
    }
    parsed_arg[args_index] = '\0';
    //check for executable
    if(read_dirEntry_by_name(parsed_cmd, &file_dentry) == -1){
        sti();
        return -1;
    }

    int data_read;
    data_read = read_data(file_dentry.INODENUM, 0, file_buf, 4);
    int temp1;
    temp1 = read_data(file_dentry.INODENUM, 24, (uint8_t*)&rtn_val, 4);
    if(data_read == -1){
        sti();
        return -1;
    }

     
    if(!(file_buf[0]==0x7F && file_buf[1]==0x45 && file_buf[2]==0x4C && file_buf[3]==0x46)){
        sti();
        return -1;
    }
    
    if (cur_terminal->task_counter >= 4){
        sti();
        printf("ERROR: reach maximum number of program per terminal\n");
        return -1;
    }
    //current_pid++;
    int pid_flag = 0;
    for (i = 0; i < MAX_NUM_PROG; i++){
        if (pid_list[i] == 0){
            current_pid = i;
            pid_list[i] = 1;
            pid_flag = 1;
            break;
        }
    }
    if (pid_flag == 0){
        sti();
        printf("ERROR: reach maximum number of pcb\n");
        return -1;
    } 
    

    //setup paging 

    paging_setup_process(current_pid);


    // Load file to memory
    read_data(file_dentry.INODENUM, 0, (uint8_t *)(0x08048000), (file_dentry.INODENUM + inodeHead)->LENGTH);


    // create PCB 
    
    // if (task_counter >= 4 || cur_terminal->task_counter >= 3){
    //     printf("ERROR: reach maximum number of tasks\n");
    //     return -1;
    // }
    
    pcb = get_current_pcb(current_pid);
    /*temp esp and ebp*/
    uint32_t temp_esp, temp_ebp, term_temp_esp, term_temp_ebp;
    
    /*load the current pid to pcb's pid and parent pid*/
    if (current_pid == 0){
        pcb->parent_pcb_pid = -1;
    }
    pcb->parent_pcb_pid = cur_terminal->pid_on_duty;
    pcb->pid = current_pid;
    pcb->terminal_id = cur_term_id;

    cur_terminal->pid_on_duty = current_pid;
    cur_terminal->task_counter++;
    task_counter++;

    /*empty the file array and initialize the first two file array as sdtin and sdtout*/
    for (i = 0; i < FILE_ARRAY_SIZE; i++){
        /*initialize pcb's file array*/
        pcb->file_array[i].flag = 0;
        pcb->file_array[i].file_pos = 0;
        if (i == 0){
            pcb->file_array[i].optable_ptr = &terminal_optable;
            pcb->file_array[i].flag = 1;
        }
        if (i == 1){
            pcb->file_array[i].optable_ptr = &terminal_optable;
            pcb->file_array[i].flag = 1;
        }

    }
    memcpy(pcb->args, parsed_arg, 128);

    // prepare for context switch 
    // tss.ss0 = KERNEL_DS;
    tss.esp0 = KERNAL_BASE - STACK_SIZE * current_pid - 4;
    
    uint32_t user_ds = USER_DS;
    uint32_t user_cs = USER_CS;
    uint32_t eip = (uint32_t) rtn_val;
    uint32_t user_s = 0x083FFFFC;
    //sti();
    if (first_shell_flag == 1){
        /*start first shell*/
        //pcb_t *pcb_temp = get_current_pcb(current_pid);
        first_shell_flag = 0;
        asm volatile (
            "movl %%ebp, %0"
            :"=r" (term_temp_ebp)
        );
        pcb->term_ebp = term_temp_ebp;
        /*save esp to the local variable*/
        asm volatile (
            "movl %%esp, %0"
            :"=r" (term_temp_esp)
        );
        pcb->term_esp = term_temp_esp;
    }
    /*save ebp to the local variable*/
    asm volatile (
        "movl %%ebp, %0"
        :"=r" (temp_ebp)
    );
    pcb->ebp = temp_ebp;
    /*save esp to the local variable*/
    asm volatile (
        "movl %%esp, %0"
        :"=r" (temp_esp)
    );
    pcb->esp = temp_esp;
    
    // push IRET to kernel stack 
    /*TRET method to go into user level, reference from osdev
     * https://wiki.osdev.org/Getting_to_Ring_3
     * esp is 0x08400000 - 4 which is 0x083FFFFC */
    asm volatile (
        
        "pushl %0               \n;"
        "pushl %1           \n;"
        "pushfl                  \n;"
        "popl %%eax               \n;"
        "orl $0x200, %%eax        \n;"
        "pushl %%eax           \n;"
        "pushl %2               \n;"
        "pushl %3               \n;"
        "iret                   \n;"
        " exec_lab:                  \n;"

        :
        : "r"(user_ds), "r"(user_s), "r"(user_cs), "r"(eip) /*need this*/
        : "memory", "cc", "eax"
    );




    sti();
    return 0;
}







/* void syscall_halt(uint8_t status)
 * Inputs: void
 * Return Value: int32_t
 * Function: halts system calls */
int32_t syscall_halt(uint8_t status){

    // cli();
    pcb_t * current_pcb;
    pcb_t * parent_pcb;
    
    current_pcb = get_current_pcb(current_pid); 
    parent_pcb= get_current_pcb(current_pcb->parent_pcb_pid);

    terminal_t *cur_terminal = &terminal_list[current_pcb->terminal_id];
    int i;
    for(i=0; i<8; i++){

        if(current_pcb->file_array[i].flag == 1){
            syscall_close(i);
        }
        
    }
    /*  Reset Current Process ID to Parent  */

    if(current_pid == 0){
        current_pid = 0;
        pid_list[current_pid] = 0;
        printf("ERROR: You are already at the basement!\n");
        syscall_execute((uint8_t *)"shell");
    } else if (cur_terminal->task_counter == 1){
        current_pid = cur_terminal->pid_on_duty;
        pid_list[current_pid] = 0;
        cur_terminal->task_counter--;
        printf("ERROR: You are already at the basement!\n");
        syscall_execute((uint8_t *)"shell");
    } else {
        cur_terminal->task_counter--;
        task_counter--;
        pid_list[current_pcb->pid] = 0;
        current_pid = parent_pcb->pid;
        cur_terminal->pid_on_duty = current_pid;
    }
    

    tss.esp0 = KERNAL_BASE - STACK_SIZE * current_pid - 4;
    // tss.ss0 = KERNEL_DS;

    /*need to halt paging and restore parent paging*/
    paging_setup_process(current_pid);
    // sti();
    clear_paging_vidmap();

    /*store the status into eax*/
    asm volatile (    
        "movl %0, %%eax   \n;"
        "movl %1, %%esp   \n;"
        "movl %2, %%ebp   \n;"
        "jmp exec_lab      \n;"
        
        :
        : "g" (status), "g" (current_pcb->esp), "g" (current_pcb->ebp)
        : "memory", "cc", "eax"
    );

    return 0;
}

/* void syscall_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: void
 * Return Value: int32_t
 * Function: reads system calls */
int32_t syscall_read (int32_t fd, void* buf, int32_t nbytes){
    /*current pcb construct*/
    pcb_t* current = get_current_pcb(current_pid);
    /*boundary check for fd to make sure its in range 0-8 except 1 for write-only*/
    if (fd < 0 || fd >= FILE_ARRAY_SIZE || fd == 1){
        //printf("ERROR: Invalid fd or stdout write-only error");
        return -1;
    }
    /*boundary check for pointer within the user value*/
    if (buf == NULL || nbytes < 0 || (int)buf < USER_START || (int)buf + nbytes > USER_END){
        //printf("ERROR: invalid buffer error");
        return -1;
    }
    /*flag check if pcb is avaialable*/
    if (current->file_array[fd].flag == 0){
        //printf("ERROR: pcb is not available");
        return -1;
    }
    return current->file_array[fd].optable_ptr->read(fd, buf, nbytes);
}


/* void syscall_open(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: void
 * Return Value: int32_t
 * Function: writes system calls */
int32_t syscall_write (int32_t fd, void* buf, int32_t nbytes){
    /*current pcb construct*/
    pcb_t* current = get_current_pcb(current_pid);
    /*boundary check for fd to make sure its in range 0-8 except 0 for read-only*/
    if (fd < 0 || fd >= FILE_ARRAY_SIZE){
        //printf("ERROR: Invalid fd or stdout read-only error");
        return -1;
    }
    /*boundary check for pointer within the user value*/
    if (buf == NULL || nbytes < 0 || (int)buf < USER_START || (int)buf + nbytes > USER_END){
        //printf("ERROR: invalid buffer error");
        return -1;
    }
    /*flag check if pcb is avaialable*/
    if (current->file_array[fd].flag == 0){
        //printf("ERROR: pcb is not available");
        return -1;
    }
    
    return current->file_array[fd].optable_ptr->write(fd, buf, nbytes);
    //return terminal_write(fd, buf, nbytes);
}


/* void syscall_open(const uint8_t* filename)
 * Inputs: void
 * Return Value: int32_t
 * Function: opens system calls */
int32_t syscall_open (const uint8_t* filename){
    dir_entry_t dentry;
    /*initialize fd to -1*/
    int32_t fd = -1;
    /*current pcb construct*/
    pcb_t* current = get_current_pcb(current_pid);
    /*boundary check for filename within the user value*/
    if (filename == NULL || strlen((int8_t*)filename) == 0){
        //printf("ERROR: invalid filename or descriptor error");
        return -1;
    }
    /*open by file name check*/
    if (read_dirEntry_by_name(filename, &dentry) == -1){
        //printf("ERROR: the filename is not exist");
        return -1;
    }

     /*find a available fd*/
    fd = find_fd(current);
    if(fd == -1){
        current->file_array[fd].flag = 0;
        return -1;

    }

    if (dentry.FILETYPE == 0){
        /*type: rtc*/
        current->file_array[fd].file_pos = 0;
        current->file_array[fd].flag = 1;
        current->file_array[fd].inode = -1;
        current->file_array[fd].optable_ptr = &rtc_optable;
    } else if (dentry.FILETYPE == 1){
        /*type: directery*/
        current->file_array[fd].file_pos = 0;
        current->file_array[fd].flag = 1;
        current->file_array[fd].inode = dentry.INODENUM;
        current->file_array[fd].optable_ptr = &dir_optable;
    } else if (dentry.FILETYPE == 2){
        /*type: file*/
        current->file_array[fd].file_pos = 0;
        current->file_array[fd].flag = 1;
        current->file_array[fd].inode = dentry.INODENUM;
        current->file_array[fd].optable_ptr = &file_optable;
    }

    current->file_array[fd].optable_ptr->open(filename);

    
    return fd;

}

/* void syscall_close(const uint8_t* filename)
 * Inputs: void
 * Return Value: int32_t
 * Function: closes system calls */
int32_t syscall_close (int32_t fd){
    /*current pcb construct*/
    pcb_t* current = get_current_pcb(current_pid);
    /*boundary check for fd to make sure its in range 1-8*/
    if (fd <= 1 || fd > FILE_ARRAY_SIZE || current->file_array == NULL){
        //printf("ERROR: Invalid fd");
        return -1;
    }
    /*flag check if pcb is avaialable*/
    if (current->file_array[fd].flag == 0){
        //printf("ERROR: pcb is not available");
        return -1;
    }
    /*change pcb status to inactive*/
    current->file_array[fd].flag = 0;
    return current->file_array[fd].optable_ptr->close(fd);

}
/* void syscall_getargs(uint8_t* buf, int32_t nbytes)
 * Inputs: void
 * Return Value: int32_t (0 or -1)
 * Function: gets arguments of syscall commands */
int32_t syscall_getargs (uint8_t* buf, int32_t nbytes){
    //cli();
    if(buf==NULL || nbytes < 0){
        return -1;
    }
    pcb_t* current_pcb = get_current_pcb(current_pid);
    
    if(current_pcb->args[0]==NULL){
        return -1;
    }
    
    //strncpy((int8_t*)buf, (int8_t*)current_pcb->args,(uint32_t)nbytes);
    memcpy(buf, current_pcb->args,(uint32_t)nbytes);
    //sti();
    int i;
    for (i = 0; i < ARGLEN; i++){
        current_pcb->args[i] = NULL;
    }
    return 0;
}


/* void syscall_vidmap(uint8_t** screenstart)
 * Inputs: void
 * Return Value: int32_t (0 or -1)
 * Function: sets up the page table for text mode */
int32_t syscall_vidmap (uint8_t** screen_start){
    /*boundary check for passing arguments*/
    if (screen_start == NULL){
        return -1;
    }
    /*the value of screen start should be inside of user spcae(128 MB to 132 MB)*/
    if ((uint32_t)screen_start < MB_128|| (uint32_t)screen_start >= MB_132){
        return -1;
    }
    paging_vidmap();
    *screen_start = (uint8_t*)(MB_132);

    return 0;
}
int32_t syscall_handler (int32_t signum, void* handler_address){
    return -1;
}
int32_t syscall_sigreturn (void){
    return -1;
}

/* void get_current_pcb(int32_t current_pid)
 * Inputs: void
 * Return Value: pcb_t
 * Function: gets the current pcb */
pcb_t* get_current_pcb(int32_t current_pid){
     return (pcb_t *)(KERNAL_BASE - (current_pid + 1) * STACK_SIZE);
}
int32_t find_fd(pcb_t *current_pcb){
    /*loop index*/
    int i;
    /*find the avialble fd*/
    for (i = 2; i < FILE_ARRAY_SIZE; i++){
        if (current_pcb->file_array[i].flag == 0){
            current_pcb->file_array[i].flag = 1;
            return i;
        }
    }

    return -1;
}


/* void paging_setup_process(int32_t pid)
 * Inputs: void
 * Return Value: void
 * Function: Allocate current pid and parent pid to the structure*/
void paging_setup_process(int32_t pid){

    /*  Set up the page_directory entry for the current program */

    /* Physical Memory Location: 8MB + (PID# * 4MB) */
    page_directory[PROCESS_INDEX].P = 1;
    page_directory[PROCESS_INDEX].US = 1;
    page_directory[PROCESS_INDEX].RW = 1;
    page_directory[PROCESS_INDEX].PS = 1;   //4mb pages for memory
    page_directory[PROCESS_INDEX].ADDR = (KERNAL_BASE + (pid * PROCESS_SIZE)) >> 12;
   /*TLB flush reference from osdev*/

    asm volatile (
        "movl %%cr3, %%eax   \n;"
        "movl %%eax, %%cr3   \n;"
        :
        :
        :"memory", "cc", "%eax"
  );

}

/* void paging_vidmap()
 * Inputs: void
 * Return Value: void
 * Function: helper function for mapping page directory and table*/
void paging_vidmap(){
     /*  Set up the page_directory entry for the current program */

    /* Physical Memory Location: 8MB + (PID# * 4MB) */
    page_directory[PROCESS_INDEX + 1].P = 1;
    page_directory[PROCESS_INDEX + 1].US = 1;
    page_directory[PROCESS_INDEX + 1].RW = 1;
    page_directory[PROCESS_INDEX + 1].PS = 0;   //4kb pages for memory
    page_directory[PROCESS_INDEX + 1].ADDR = (((uint32_t) &vidmap_page_table) >> 12);
    vidmap_page_table[0].US = 1;
    vidmap_page_table[0].ADDR = PVIDEOLOCATION;
    vidmap_page_table[0].P = 1;
   /*TLB flush reference from osdev*/
    asm volatile (
        "movl %%cr3, %%eax  \n;"
        "movl %%eax, %%cr3  \n;"
        :
        :
        :"memory", "cc", "%eax"
  );
}


/* void clear_paging_vidmap()
 * Inputs: void
 * Return Value: void
 * Function: helper function for resetting vidmap values*/
void clear_paging_vidmap(){
     /*  Set up the page_directory entry for the current program */

    /* Physical Memory Location: 8MB + (PID# * 4MB) */
    page_directory[PROCESS_INDEX + 1].P = 0;
    page_directory[PROCESS_INDEX + 1].US = 0;
    page_directory[PROCESS_INDEX + 1].RW = 0;
    page_directory[PROCESS_INDEX + 1].PS = 0;   //4kb pages for memory
    page_directory[PROCESS_INDEX + 1].ADDR = 0;
    vidmap_page_table[0].US = 0;
    vidmap_page_table[0].ADDR = 0;
    vidmap_page_table[0].P = 0;
   
}


/* void optable_init()
 * Inputs: void
 * Return Value: void
 * Function: table for calling different read/write/open/close functions*/
void optable_init()
{
    terminal_optable.open = &terminal_open;
    terminal_optable.read = &terminal_read;
    terminal_optable.write = &terminal_write;
    terminal_optable.close = &terminal_close;

    file_optable.open = &fileOpen;
    file_optable.close = &fileClose;
    file_optable.read = &fileRead;
    file_optable.write = &fileWrite;

    rtc_optable.open = &rtc_open;
    rtc_optable.close = &rtc_close;
    rtc_optable.read = &rtc_read;
    rtc_optable.write = &rtc_write;

    dir_optable.open = &dirEntryOpen;
    dir_optable.close = &dirEntryClose;
    dir_optable.read = &dirEntryRead;
    dir_optable.write = &dirEntryWrite;
  
}




