# pagingHelp.S - assembly code from osdev that actually enables paging
# vim:ts=4 noexpandtab

#define ASM     1

.text
.globl enable_paging
# void enable_paging(void)
#  Inputs: location for page directory
#  Return Value: void
# Function: enables 4 kb pages by setting up cr4 with the correct values, loads page directory into cr3, enables paging by setting correct bit in cr0
enable_paging:
    push %ebp
    movl %esp, %ebp
    
    #   first enable 4kb pages by setting up cr4
    #   need to set the 4th bit to enable 4kbpages
    movl %cr4, %eax
    orl $0x00000010, %eax
    movl %eax, %cr4

    
    #   https://wiki.osdev.org/Setting_Up_Paging
    movl 8(%esp), %eax
    movl %eax, %cr3

    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    leave
    ret
