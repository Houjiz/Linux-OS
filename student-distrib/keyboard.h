#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_MAP_SIZE   58
#define KEYBOARD_BUF_SIZE   128
#define KEY_PORT            0x60
#define KEY_IRQ             0x01

#define CTRL                0x1D   
#define RELEASE_CTRL        0x9D

#define LEFT_SHIFT          0x2A
#define RIGHT_SHIFT         0x36
#define RELEASE_LEFT_SHIFT  0xAA
#define RELEASE_RIGHT_SHIFT 0xB6

#define ALT                 0x38 
#define RELEASE_ALT         0xB8

#define CAPSLOCK            0x3A
#define ENTER               0x1C
#define BACKSPACE           0x0E    
#define TAB                 0x0F
#define L_PRESSED           0x26

#define F1                  0x3B
#define F2                  0x3C
#define F3                  0x3D

extern void keyboard_init();
extern void keyboard_handler();

extern int enter_flag;
extern int enter_sc_flag;
extern int keyboard_buf_length;
extern unsigned char keyboard_buf[KEYBOARD_BUF_SIZE];
extern unsigned char temp_kb_buf[KEYBOARD_BUF_SIZE];
extern int treminal_read_flag;

#endif



