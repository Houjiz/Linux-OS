#include "rtc.h"
#include "i8259.h"
#include "lib.h"
#include "types.h"



volatile int flag = 0;

/* void rtc_init(void)
 * Inputs: void
 * Return Value: void
 * Function: Initialize the rtc and enables interrupts from it */
void rtc_init(){
    cli();
    outb(0x8B, 0x70); // select register and disable interrupts
    char temp;
    temp = inb(0x71);
    outb(0x8B, 0x70); 
    outb(temp | 0x40, 0x71); // turn on bit 6 of registers 
    enable_irq(8);
    sti();
}

/* void rtc_interrupt_handler(void)
 * Inputs: void
 * Return Value: void
 * Function: handles interrupts from RTC */
void rtc_interrupt_handler(){

    cli();
    //test_interrupts(); // calling function to test RTC
    outb(0x0C, 0x70);
    inb(0x71);
    flag = 1;
    send_eoi(8);
    sti();
}

/* int rtc_open(void)
 * Inputs: void
 * Return Value: 0 (int)
 * Function: Open the RTC */
int rtc_open(){
    int rate;
    rate = 0x0F;
     cli();
    outb(0x8A, 0x70); // select register and disable interrupts
    char temp;
    temp = inb(0x71);
    outb(0x8A, 0x70); 
    outb((temp & 0xF0) | rate , 0x71); 
    sti();
    return 0;

}

/* int rtc_open(void)
 * Inputs: void
 * Return Value: 0 (int)
 * Function: Close the RTC */
int rtc_close(){
    return 0;
}

/* int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes)
 * Inputs: void
 * Return Value: -1 if not valid, else 0 (int)
 * Function: Handles the frequency, twice as fast rate for increasing frequency */
int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes){

    int rate, frequency;
    //frequency = (int) buf;
    int *result = (int*) buf;
    frequency = *result;
    // bounds check
    if(frequency < 2 || frequency > 1024 || frequency == NULL){
        return -1; 
    }

    int i;
    int counter = 1;
    
    // log 2 algorithm for calculating rate
    for (i=0;i<10;i++){

        if (frequency == 2) {
            break;
        }
        frequency = frequency/2;
        counter++;
    }
     rate = 16 - counter;


    cli();
    outb(0x8A, 0x70); 
    char temp;
    temp = inb(0x71);
    outb(0x8A, 0x70); 
    outb((temp & 0xF0) | rate , 0x71); 
    sti();

    return nbytes;

}


/* int rtc_read(void)
 * Inputs: void
 * Return Value: 0 (int)
 * Function: Until an interrupt is received, RTC is blocked */
int rtc_read(){

    while(!flag);
    flag=0;
    return 0;


}

