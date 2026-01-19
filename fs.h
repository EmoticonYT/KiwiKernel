#ifndef FS_H
#define FS_H

#define MAX_FILES 64
#define MAX_FILE_SIZE 1024

typedef enum { FT_TEXT, FT_SCRIPT, FT_DIR } file_type_t;

typedef struct {
    char name[32];
    char parent[64];
    char content[MAX_FILE_SIZE];
    int exists;
    int is_dir;
    file_type_t type;
} file_t;

file_t filesystem[MAX_FILES];
char current_path[64] = "/";

void init_fs() {
    for(int i = 0; i < MAX_FILES; i++) {
        filesystem[i].exists = 0;
    }

    filesystem[0].exists = 1;
    filesystem[0].type = FT_SCRIPT;
    const char* name = "sysinfo.sh";
    for(int n=0; name[n]; n++) filesystem[0].name[n] = name[n];
    filesystem[0].parent[0] = '/';
    filesystem[0].parent[1] = '\0';
    
    const char* script = "#!kiwi\nversion\nbirdfetch\nls\n";
    for(int n=0; script[n]; n++) filesystem[0].content[n] = script[n];
}

#endif