#include "screen.h"
#include "fs.h"
#include "ke.h"

char command_buffer[256];
int command_index = 0;
static int shift_active = 0;

void visual_backspace() {
    if (cursor_pos > 0) {
        cursor_pos--;
        video_memory[cursor_pos * 2] = ' ';
        video_memory[cursor_pos * 2 + 1] = 0x07;
        update_cursor();
    }
}

const char ascii_map[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t', 
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0
};

const char shift_map[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t', 
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S', 
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0
};

void kmain() {
    init_fs();
    clear_screen();
    kprint("Kiwi Kernel v0.1.4 Support Booted.\nkiwi:/>> ");

    while(1) {
        if (port_byte_in(0x64) & 0x01) {
            unsigned char scancode = port_byte_in(0x60);
            
            // Explicitly catch Shift status before processing any other keys
            if (scancode == 0x2A || scancode == 0x36) {
                shift_active = 1;
                continue; 
            } 
            if (scancode == 0xAA || scancode == 0xB6) {
                shift_active = 0;
                continue;
            }

            if (scancode < 0x80) {
                char c = shift_active ? shift_map[scancode] : ascii_map[scancode];
                if (c == '\n') {
                    kprint_char('\n');
                    command_buffer[command_index] = '\0';
                    execute_command(command_buffer, command_index);
                    command_index = 0;
                } else if (c == '\b') {
                    if (command_index > 0) {
                        command_index--;
                        visual_backspace();
                    }
                } else if (c != 0) {
                    kprint_char(c);
                    if (command_index < 255) command_buffer[command_index++] = c;
                }
            }
        }
    }
}