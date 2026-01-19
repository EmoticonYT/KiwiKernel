#ifndef KE_H
#define KE_H

#include "screen.h"
#include "fs.h"

/* 1. Helper: String Comparison */
int strcmp_nocase(char *s1, char *s2) {
    int i = 0;
    while (s1[i] != '\0' || s2[i] != '\0') {
        char c1 = s1[i], c2 = s2[i];
        if (c1 >= 'a' && c1 <= 'z') c1 -= 32;
        if (c2 >= 'a' && c2 <= 'z') c2 -= 32;
        if (c1 != c2) return 0;
        i++;
    }
    return 1;
}

/* 2. Hardware: Get CPU Vendor String */
void get_cpu_vendor(char* vendor) {
    unsigned int ebx, ecx, edx;
    __asm__ ("cpuid" : "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    for(int i = 0; i < 4; i++) vendor[i] = (ebx >> (i * 8)) & 0xFF;
    for(int i = 0; i < 4; i++) vendor[i + 4] = (edx >> (i * 8)) & 0xFF;
    for(int i = 0; i < 4; i++) vendor[i + 8] = (ecx >> (i * 8)) & 0xFF;
    vendor[12] = '\0';
}

/* 3. Hardware: Read CMOS Memory */
unsigned short get_base_memory() {
    unsigned short total;
    unsigned char low, high;
    port_byte_out(0x70, 0x15);
    low = port_byte_in(0x71);
    port_byte_out(0x70, 0x16);
    high = port_byte_in(0x71);
    total = low | (high << 8);
    return total;
}

/* 4. Tool: Birdfetch */
void birdfetch() {
    char vendor[13];
    get_cpu_vendor(vendor);
    unsigned short mem = get_base_memory();
    kprint("    ____      OS: Kiwi Kernel v0.1.4\n");
    kprint("   /    \\     CPU: "); kprint(vendor); kprint("\n");
    kprint(" (  Kiwi )    RAM: "); 
    if (mem > 0) {
        char mstr[8]; int m = mem, i = 0;
        while(m > 0) { mstr[i++] = (m % 10) + '0'; m /= 10; }
        while(i > 0) kprint_char(mstr[--i]);
        kprint(" KB\n");
    }
    kprint("   \\____/     Shell: KE Shell 1.2\n");
    kprint("    |  |      Display: VGA 80x25\n");
}

/* 5. Logic: Execute Command (Prototype for Recursion) */
void execute_command(char* buffer, int len);

/* 6. Logic: Script Runner */
void run_script(int idx) {
    if (filesystem[idx].content[0] == '#' && filesystem[idx].content[1] == '!') {
        char line[128];
        int lp = 0, i = 0;
        while(filesystem[idx].content[i] != '\n' && filesystem[idx].content[i] != '\0') i++;
        if(filesystem[idx].content[i] == '\n') i++;
        while (filesystem[idx].content[i] != '\0') {
            if (filesystem[idx].content[i] == '\n') {
                line[lp] = '\0';
                if (lp > 0) execute_command(line, lp);
                lp = 0;
            } else { line[lp++] = filesystem[idx].content[i]; }
            i++;
        }
    }
}

/* 7. Main Shell Entry */
void execute_command(char* buffer, int len) {
    (void)len;
    char cmd[32] = {0}, arg[32] = {0};
    int i = 0;
    while (buffer[i] == ' ') i++;
    int cmd_ptr = 0;
    while (buffer[i] != ' ' && buffer[i] != '\0' && cmd_ptr < 31) cmd[cmd_ptr++] = buffer[i++];
    while (buffer[i] == ' ') i++;
    int arg_ptr = 0;
    while (buffer[i] != '\0' && arg_ptr < 31) arg[arg_ptr++] = buffer[i++];

    if (cmd[0] == '\0') return;

    int found = 0;
    if (strcmp_nocase(cmd, "BIRDFETCH")) { found = 1; birdfetch(); }
    else if (strcmp_nocase(cmd, "HELP")) {
        found = 1;
        kprint("Commands: ls, cd, mkdir, cat, touch, sh, birdfetch, version, clear, reboot\n");
    }
    else if (strcmp_nocase(cmd, "LS")) {
        found = 1;
        for(int f=0; f<MAX_FILES; f++) {
            if(filesystem[f].exists) {
                if(filesystem[f].is_dir) kprint("[DIR] ");
                else kprint("[FILE] ");
                kprint(filesystem[f].name); kprint("\n");
            }
        }
    }
    else if (strcmp_nocase(cmd, "CD")) {
        found = 1;
        if (arg[0] == '\0' || (arg[0] == '.' && arg[1] == '.')) {
            current_path[0] = '/'; current_path[1] = '\0';
        } else {
            int d_idx = -1;
            for(int f=0; f<MAX_FILES; f++) {
                if(filesystem[f].exists && filesystem[f].is_dir && strcmp_nocase(filesystem[f].name, arg)) {
                    d_idx = f; break;
                }
            }
            if(d_idx != -1) {
                current_path[0] = '/';
                int n = 0; while(arg[n]) { current_path[n+1] = arg[n]; n++; }
                current_path[n+1] = '\0';
            } else kprint("Directory not found.\n");
        }
    }
    else if (strcmp_nocase(cmd, "MKDIR")) {
        found = 1;
        int slot = -1;
        for(int f=0; f<MAX_FILES; f++) { if(!filesystem[f].exists) { slot = f; break; } }
        if (slot != -1) {
            filesystem[slot].exists = 1;
            filesystem[slot].is_dir = 1;
            for(int n=0; arg[n] && n<31; n++) filesystem[slot].name[n] = arg[n];
        } else kprint("FS Full.\n");
    }
    else if (strcmp_nocase(cmd, "CAT")) {
        found = 1;
        int f_idx = -1;
        for(int f=0; f<MAX_FILES; f++) {
            if(filesystem[f].exists && !filesystem[f].is_dir && strcmp_nocase(filesystem[f].name, arg)) { f_idx = f; break; }
        }
        if (f_idx != -1) { kprint(filesystem[f_idx].content); kprint("\n"); }
        else kprint("File not found.\n");
    }
    else if (strcmp_nocase(cmd, "TOUCH")) {
        found = 1;
        int slot = -1;
        for(int f=0; f<MAX_FILES; f++) { if(!filesystem[f].exists) { slot = f; break; } }
        if (slot != -1) {
            filesystem[slot].exists = 1;
            filesystem[slot].is_dir = 0;
            for(int n=0; arg[n] && n<31; n++) filesystem[slot].name[n] = arg[n];
            filesystem[slot].content[0] = '\0';
        } else kprint("FS Full.\n");
    }
    else if (strcmp_nocase(cmd, "SH")) {
        found = 1;
        int s_idx = -1;
        for(int f=0; f<MAX_FILES; f++) {
            if(filesystem[f].exists && strcmp_nocase(filesystem[f].name, arg)) { s_idx = f; break; }
        }
        if (s_idx != -1) run_script(s_idx);
        else kprint("Script not found.\n");
    }
    else if (strcmp_nocase(cmd, "VERSION")) { found = 1; kprint("Kiwi Kernel v0.1.4\n"); }
    else if (strcmp_nocase(cmd, "CLEAR")) { found = 1; clear_screen(); }
    else if (strcmp_nocase(cmd, "REBOOT")) { found = 1; port_byte_out(0x64, 0xFE); }

    if (!found) kprint("Command not recognized...\n");
    kprint("kiwi:"); kprint(current_path); kprint(">> ");
}

#endif