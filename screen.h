#ifndef SCREEN_H
#define SCREEN_H

#include "io.h"

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F

int cursor_pos = 0;
volatile char *video_memory = (volatile char*) VIDEO_ADDRESS;

void update_cursor() {
    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (unsigned char)(cursor_pos & 0xFF));
    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (unsigned char)((cursor_pos >> 8) & 0xFF));
}

void clear_screen() {
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }
    cursor_pos = 0;
    update_cursor();
}

void kprint_char(char c) {
    if (c == '\n') {
        cursor_pos = (cursor_pos / MAX_COLS + 1) * MAX_COLS;
    } else {
        video_memory[cursor_pos * 2] = c;
        video_memory[cursor_pos * 2 + 1] = WHITE_ON_BLACK;
        cursor_pos++;
    }
    update_cursor();
}

void kprint(char *str) {
    for (int i = 0; str[i] != '\0'; i++) kprint_char(str[i]);
}

#endif
