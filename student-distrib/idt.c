#include "idt.h"
#include "x86_desc.h"
#include "keyboard.h"
#include "rtc.h"
#include "interrupts.h"
#include "lib.h"
#include "syscall.h"
#include "schedule.h"


/*

Exception/Interrupt handler stuff.
These are really just gonna print stuff and lock down the OS
Due to this, we don't really need to worry about documentation.

*/

 void divideerror() {

    printf("Dividing Error Exception!");
    syscall_halt((uint8_t)256);

 }

 void debug() {

    printf("Debug Exception!");
    syscall_halt((uint8_t)256);

 }

 void nmi_interrupt() {

    printf("NMI Interrupt Received");
    syscall_halt((uint8_t)256);

 }

 void breakpoint() {

    printf("Breakpoint Exception!");
    syscall_halt((uint8_t)256);

 }

 void overflow() {

    printf("Overflow Exception!");
    syscall_halt((uint8_t)256);

 }

 void boundrange() {

    printf("BOUND Range Exceeded Exception!");
    syscall_halt((uint8_t)256);

 }

 void invalopcode() {

    printf("Invalid Opcode Exception!");
    syscall_halt((uint8_t)256);

 }

 void devnotavail() {

    printf("Device Not Available Exception!");
    syscall_halt((uint8_t)256);

 }

 void doublefault() {

    printf("Double Fault Exeption!");
    syscall_halt((uint8_t)256);

 }

 void coprocessseg() {

    printf("Coprocessor Segment Overrun!");
    syscall_halt((uint8_t)256);

 }

 void invaltss() {

    printf("Invalid TSS Exception!");
    syscall_halt((uint8_t)256);

 }

 void segnotpres() {

    printf("Segment Not Present");
    syscall_halt((uint8_t)256);

 }

 void stackfault() {

    printf("Stack Fault Exception!");
    syscall_halt((uint8_t)256);

 }

 void genprotection() {

    printf("General Protection Exception!");
    syscall_halt((uint8_t)256);

 }

 void pagefault() {

    printf("Page-Fault Exception!");
   syscall_halt((uint8_t)256);
   // while(1);
 }

 void fpufloatpoint() {

    printf("FPU Floating-Point Error!");
    syscall_halt((uint8_t)256);

 }

 void alligncheck() {

    printf("Alignment Check Exception!");
    syscall_halt((uint8_t)256);

 }

 void machinecheck() {

    printf("Machine-Check Exception!");
    syscall_halt((uint8_t)256);

 }

 void simdfloatpoint() {

    printf("SIMD Floating-Point Exception!");
    syscall_halt((uint8_t)256);

 }
// place holder for system calls (IDT entry)
void sysCallHandler() {

    printf("Syscall");

 }

/* void setup_idt(void)
 * Inputs: void
 * Return Value: void
 * Function: Initialize the idt entries with proper values for each exception call */
 void setup_idt() {

    /*  iterator variable   */
    uint32_t i = 0;

    for(i = 0; i < NUM_VEC; i++) {
            //from documentation: each idt... should set this field to be the kernel's code segment selector
            idt[i].seg_selector = KERNEL_CS;
            idt[i].reserved4 = 0;
            idt[i].reserved3 = 1;
            idt[i].reserved2 = 1;
            idt[i].reserved1 = 1;
            idt[i].size = 1;
            idt[i].reserved0 = 0;
            idt[i].dpl = 0;
            idt[i].present = 1;

    }

    /*  Setting up the sys call handler */
    //privelege set so users can use it
    idt[SYSCALLENTRY].dpl = 3;

    /*  Keyboard    */
    idt[KEYLOCAL].reserved3 = 0;

    // /*  RTC         */
    idt[RTCLOCAL].reserved3 = 0;

    // /*  PIT         */
    idt[PITLOCAL].reserved3 = 0;


 SET_IDT_ENTRY(idt[DIVERROR], divideerror);
 SET_IDT_ENTRY(idt[DEBUGEX], debug);
 SET_IDT_ENTRY(idt[NMI], nmi_interrupt);
 SET_IDT_ENTRY(idt[BREAKPT], breakpoint);
 SET_IDT_ENTRY(idt[OVERFLOW], overflow);
 SET_IDT_ENTRY(idt[BOUND], boundrange);
 SET_IDT_ENTRY(idt[INVOP], invalopcode);
 SET_IDT_ENTRY(idt[DEVNOTAVAIL], devnotavail);
 SET_IDT_ENTRY(idt[DOUBLFAULT], doublefault);
 SET_IDT_ENTRY(idt[COPSEGO], coprocessseg);
 SET_IDT_ENTRY(idt[INVALTSS], invaltss);
 SET_IDT_ENTRY(idt[SEGNOTP], segnotpres);
 SET_IDT_ENTRY(idt[STACKFAULT], stackfault);
 SET_IDT_ENTRY(idt[GENPROT], genprotection);
 SET_IDT_ENTRY(idt[PAGEFAULT], pagefault);
 SET_IDT_ENTRY(idt[FLOATPOINT], fpufloatpoint);
 SET_IDT_ENTRY(idt[ALIGNCHECK], alligncheck);
 SET_IDT_ENTRY(idt[MACHCHECK], machinecheck);
 SET_IDT_ENTRY(idt[SIMDFLOAT], simdfloatpoint);
 SET_IDT_ENTRY(idt[SYSCALLENTRY], sysCallHandler);
// linkage for testing RTC and Keyboard 
 SET_IDT_ENTRY(idt[KEYLOCAL], name);
 SET_IDT_ENTRY(idt[RTCLOCAL], name2);
 SET_IDT_ENTRY(idt[SYSCALLLOCAL],name3);
 SET_IDT_ENTRY(idt[PITLOCAL],name4);
}




