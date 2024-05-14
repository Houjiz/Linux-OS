#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "terminal.h"

/*keyboard lower case mapping*/
unsigned char keyboard_LC_mapping[KEYBOARD_MAP_SIZE] = {
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8',  '9', '0', '-','=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',  'p', '[', ']', '\n', '\0', 
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  '\0', '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\0', '\0',  '\0', ' ' 
};
/*keyboard upper case mapping
 *using when shift is pressing, do not care capslock*/
unsigned char keyboard_UC_mapping[KEYBOARD_MAP_SIZE] = {
    '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*',  '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',  'P', '{', '}', '\n', '\0',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', '\0', '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', '\0', '\0', '\0', ' '
};
/*keyboard mapping for capslock on but */
unsigned char keyboard_CAPS_mapping[KEYBOARD_MAP_SIZE] = {
    '\0', '\0', '1', '2', '3', '4', '5', '6', '7', '8',  '9', '0', '-','=', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'T', 'U', 'I', 'O',  'P', '[', ']', '\n', '\0', 
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',  '\0', '\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', '\0', '\0',  '\0', ' ' 
};
unsigned char keyboard_CB_mapping[KEYBOARD_MAP_SIZE] = {
    '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*',  '(', ')', '_', '+', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',  'p', '{', '}', '\n', '\0', 
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~',  '\0', '|',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', '\0', '\0',  '\0', ' ' 
};

int ctrl_flag = 0;
int shift_flag = 0;
int alt_flag = 0;
int capslock_flag = 0; 
int enter_flag = 0;
int enter_sc_flag = 0;
int treminal_read_flag = 0;

unsigned char keyboard_buf[KEYBOARD_BUF_SIZE];
int keyboard_buf_length = 0;
unsigned char temp_kb_buf[KEYBOARD_BUF_SIZE];


/* void keyboard_handler(void)
 * Inputs: void
 * Return Value: void
 * Function: handles the keyboard interrupts, prints the correct char */
void keyboard_handler(){
    terminal_t *cur_terminal = &terminal_list[cur_term_id];
    int i;
    unsigned char scanline;
    unsigned char key;
    scanline = inb(KEY_PORT); // takes the key inputted by the user
    
    /*special key handling*/
    if (scanline == CTRL){
        ctrl_flag = 1;
    }
    if (scanline == RELEASE_CTRL){
        ctrl_flag = 0;
    }
    if (scanline == ALT){
        alt_flag = 1;
    }
    if (scanline == RELEASE_ALT){
        alt_flag = 0;
    }
    if (scanline == LEFT_SHIFT || scanline == RIGHT_SHIFT){
        shift_flag = 1;
    }
    if (scanline == RELEASE_LEFT_SHIFT || scanline == RELEASE_RIGHT_SHIFT){
        shift_flag = 0;
    }
    if (scanline == CAPSLOCK){
        if (capslock_flag == 0){
            capslock_flag = 1;
            //printf("caps");
        } else {
            capslock_flag = 0;
        }
    }
    
    /*ctrl + l or L to clear screen*/
    if (ctrl_flag == 1 && scanline == 0x26){
        clear();
        for (i = 0; i < KEYBOARD_BUF_SIZE; i++){
            cur_terminal->keyboard_buf[i] = '\0';
            keyboard_buf[i] = '\0';
        }
        cur_terminal->keyboard_buf_length = 0;
        keyboard_buf_length = 0;
        send_eoi(KEY_IRQ);
        return;
    }
    
    /*handle terminal switch*/
    if (alt_flag == 1 && scanline == F1){
        send_eoi(KEY_IRQ);
        terminal_switch(0);
        return;
    }
    if (alt_flag == 1 && scanline == F2){
        send_eoi(KEY_IRQ);
        terminal_switch(1);
        return;
    }
    if (alt_flag == 1 && scanline == F3){
        send_eoi(KEY_IRQ);
        terminal_switch(2);
        return;
    }
    
    
    /*handle enter*/
    if (scanline == ENTER){
        enter_sc_flag = 1;
        enter_flag = 1;
        cur_terminal->enter_flag = 1;
        for (i = 0; i < KEYBOARD_BUF_SIZE; i++){
            temp_kb_buf[i] = keyboard_buf[i];
        }
        // for (i = 0; i < keyboard_buf_length; i++){
        //     cur_terminal->keyboard_buf[i] = '\0';
        //     keyboard_buf[i] = '\0';
        // }
        
        
        putc('\n');
        keyboard_buf_length = 0;
        cur_terminal->keyboard_buf_length = 0;
        send_eoi(KEY_IRQ);
        return;
        //printf("inside if enter flag is %d\n", enter_flag);
    }
    //printf("outside if enter flag is %d\n", enter_flag);
    /*boundary check*/
    if (scanline >= 58){
        send_eoi(KEY_IRQ);
        return;
    }
    
    if (shift_flag == 1 && capslock_flag == 1){
        /*case: if shift and capslock are pressed*/
        key = keyboard_CB_mapping[scanline];
    } else if (shift_flag == 1 && capslock_flag == 0){
        /*case: if shif is pressed and capslock is pressed*/
        key = keyboard_UC_mapping[scanline];
    } else if (shift_flag == 0 && capslock_flag == 1){
        /*case: if shift is not pressed and capslock is pressed*/
        key = keyboard_CAPS_mapping[scanline];
    } else if (shift_flag == 0 && capslock_flag == 0){
        /*default case*/
        key = keyboard_LC_mapping[scanline];
    }
    //printf("kb length%d\n", keyboard_buf_length);
    /*handle tab*/

    
    if (key == '\t'){
        for (i = 0; i < 4; i++){
            keyboard_buf[keyboard_buf_length] = ' ';
            keyboard_buf_length += 1;
            cur_terminal->keyboard_buf[cur_terminal->keyboard_buf_length] = ' ';
            cur_terminal->keyboard_buf_length += 1;
            //printf("kb length%d\n", keyboard_buf_length);
        }
        putc('\t');
        send_eoi(KEY_IRQ);
        return;
        
    }
    //printf("kb length%d\n", keyboard_buf_length);
    /*handle backspace*/
    if (key == '\b'){
        /*check if the buffer is empty*/
        //printf("get into this ");
        if (keyboard_buf_length != 0){
            /*check if last char is tab*/
            putc('\b');
            keyboard_buf[keyboard_buf_length] = '\0';
            keyboard_buf_length -= 1;
            cur_terminal->keyboard_buf[cur_terminal->keyboard_buf_length] = '\0';
            cur_terminal->keyboard_buf_length -= 1;
            send_eoi(KEY_IRQ);
            return;
        } else {
            send_eoi(KEY_IRQ);
            return;
        }
    }
    //printf("or kb length%d\n", keyboard_buf_length);
    /*default case*/
    /*check if buffer is overflow*/
    if (keyboard_buf_length < KEYBOARD_BUF_SIZE){
        if (treminal_read_flag){
            send_eoi(KEY_IRQ);
            return;
        }
        if (scanline == LEFT_SHIFT || scanline == RIGHT_SHIFT){
        send_eoi(KEY_IRQ);
        return;
        }
        if (scanline == ALT){
            send_eoi(KEY_IRQ);
            return;
        }
        if (scanline == CTRL){
            send_eoi(KEY_IRQ);
            return;
        }
        keyboard_buf[keyboard_buf_length] = key;
        keyboard_buf_length += 1;
        cur_terminal->keyboard_buf[cur_terminal->keyboard_buf_length] = key;
        cur_terminal->keyboard_buf_length += 1;
        putc(key);
    }
    

    // if it exists in the map
    /*if(scanline < 52){
        
        putc(keyboard_map[scanline]);
        
    }
    */
    
    send_eoi(KEY_IRQ);

    return;
  
       
}

/* void keyboard_init(void)
 * Inputs: void
 * Return Value: void
 * Function: Initialize the keyboard, enables keyboard interrupts as well */
void keyboard_init(){
    enable_irq(KEY_IRQ);
}


