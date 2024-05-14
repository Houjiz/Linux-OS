/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* void i8259_init(void)
 * Inputs: void
 * Return Value: void
 * Function: Initialize the 8259 PIC */
void i8259_init(void) {
    outb(ICW1,MASTER_8259_PORT); 
    //io_wait();
    outb(ICW1,SLAVE_8259_PORT);
    //io_wait();
    outb(ICW2_MASTER,MASTER_8259_PORT+1); // mapping to data port (hence the plus 1)
    //io_wait();
    outb(ICW2_SLAVE,SLAVE_8259_PORT+1);
    //io_wait();
    outb(ICW3_MASTER,MASTER_8259_PORT+1);
    //io_wait();
    outb(ICW3_SLAVE,SLAVE_8259_PORT+1);
    //io_wait();
    outb(ICW4,MASTER_8259_PORT+1);
    //io_wait();
    outb(ICW4,SLAVE_8259_PORT+1);
    //io_wait();
    outb(master_mask, MASTER_8259_PORT+1);
    outb(slave_mask, SLAVE_8259_PORT+1);

    enable_irq(2); 
}

/* void enable_irq(irq_num)
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Function: Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    
    uint16_t port;
    uint8_t value;
    // less than 8 for primary PIC
    if (irq_num <8){
        port = MASTER_8259_PORT+1;
       
    }
    // else slave PIC
    else {
        port = SLAVE_8259_PORT+1;
        irq_num-=8;
       
    }
    // removing the mask
    value=inb(port) & ~(1 << irq_num);
    outb(value, port);
}

/* void disable_irq(irq_num)
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Function: Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
   
    uint16_t port;
    uint8_t value;
    // less than 8 for primary PIC
    if (irq_num <8){
        port = MASTER_8259_PORT+1;
        
    }
    // else slave PIC
    else {
        port = SLAVE_8259_PORT+1;
        irq_num-=8;
       
    }
    // applying the mask
    value=inb(port) | (1 << irq_num);
    outb(value, port);
}
/* void send_eoi(irq_num)
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Function: Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // less than 8 for primary PIC
    if (irq_num < 8){
        outb(EOI | irq_num, MASTER_8259_PORT);
    }
    // else slave PIC
    else{
        outb(EOI | (irq_num-8), SLAVE_8259_PORT);
        outb(EOI | 2, MASTER_8259_PORT);
    }
    // let computer know that it is ready for another interrupt from the same device
}


