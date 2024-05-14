.text 


.global name                      
.align 4
# linkage for keyboard_handler
name:                               
    pushal                          
    pushfl                          
    call keyboard_handler                   
    popfl                           
    popal                           
    iret                            

.global name2                         
.align 4
# linkage for rtc_interrupt_handler
name2:                              
    pushal                          
    pushfl                          
    call rtc_interrupt_handler                   
    popfl                           
    popal                           
    iret                            

.global name4                         
.align 4
# linkage for pit_interrupt_handler
name4:                              
    pushal                          
    pushfl                          
    call pit_interrupt_handler                   
    popfl                           
    popal                           
    iret   



syscall_jumptable:
    .long 0x0, syscall_halt, syscall_execute, syscall_read, syscall_write, syscall_open, syscall_close, syscall_getargs, syscall_vidmap, syscall_handler, syscall_sigreturn

.global name3                      
.align 4
# linkage for syscall_jumptable
name3:                            
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi
    pushl %ebp
    pushl %esp
    pushfl
    cmpl $1, %eax
    jl exit
    cmpl $10, %eax
    jg exit
    pushl %edx
    pushl %ecx
    pushl %ebx
    call *syscall_jumptable(,%eax,4)
    popl %ebx
    popl %ecx
    popl %edx

leave_function:
    popfl
    popl %esp
    popl %ebp
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    iret

exit: 
    movl $-1, %eax
    jmp leave_function




