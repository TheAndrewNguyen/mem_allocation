#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static char mem[MEMSIZE];

// initalize the memory 
// blank slate 
void mem_init(void) {
    for (int i = 0; i < MEMSIZE; ++i) mem[i] = '.';
}

// clear the memory 
void mem_clear(void) { mem_init(); }

// 
void mem_show(void) {
    for (int i = 0; i < MEMSIZE; ++i) putchar(mem[i]);
    putchar('\n');
}

// free / allocated segments struct 
typedef struct {
    int start;
    int len;
} seg_t;

// find all free segments
// returns count of free segments 
static int find_free_segments(seg_t *segments, int max_segments) {
    int count = 0;
    int i = 0;
    while (i < MEMSIZE) {
        if (mem[i] == '.') {
            int s = i;
            while (i < MEMSIZE && mem[i] == '.') ++i;
            int length = i - s;
            if (count < max_segments) {
                segments[count].start = s;
                segments[count].len = length;
            }
            ++count;
        } else {
            ++i;
        }
    }
    return count;
}

// allocate memory based on algo 
bool mem_allocate(char name, int size, char algo) {
    if (size <= 0 || size > MEMSIZE) {
        printf("Error: invalid size %d\n", size);
        return false;
    }
    seg_t segs[MEMSIZE];
    int segcount = find_free_segments(segs, MEMSIZE);

    int chosen_idx = -1;

    
    if (algo == 'F') {
        for (int i = 0; i < segcount; ++i) {
            if (segs[i].len >= size) { chosen_idx = i; break; }
        }

    } else if (algo == 'B') {
        int best_len = MEMSIZE + 1;
        for (int i = 0; i < segcount; ++i) {
            if (segs[i].len >= size && segs[i].len < best_len) {
                best_len = segs[i].len;
                chosen_idx = i;
            }
        }
    } else if (algo == 'W') {
        int worst_len = -1;
        for (int i = 0; i < segcount; ++i) {
            if (segs[i].len >= size && segs[i].len > worst_len) {
                worst_len = segs[i].len;
                chosen_idx = i;
            }
        }
    } else {
        printf("Error: unknown algorithm '%c'\n", algo);
        return false;
    }

    if (chosen_idx == -1) {
        printf("Error: Not enough memory for process %c of size %d using %c\n", name, size, algo);
        return false;
    }

    int start = segs[chosen_idx].start;
    for (int i = start; i < start + size; ++i) mem[i] = name;
    return true;
}

// free memory
// place .
void mem_free(char name) {
    bool found = false;
    for (int i = 0; i < MEMSIZE; ++i) {
        if (mem[i] == name) {
            mem[i] = '.';
            found = true;
        }
    }
    if (!found) {
        printf("Warning: process %c not found.\n", name);
    }
}

//Compact: move all non-dot bytes left preserving their block orde
void mem_compact(void) {
    char compacted[MEMSIZE];
    int write = 0;
    int i = 0;

    while (i < MEMSIZE) {
        if (mem[i] != '.') {
            char ch = mem[i];
            int j = i;
            while (j < MEMSIZE && mem[j] == ch) ++j;
            int block_len = j - i;
            /* write the block */
            for (int k = 0; k < block_len; ++k) {
                compacted[write++] = ch;
            }
            i = j;
        } else {
            ++i;
        }
    }
    //fill remainder with '.' 
    while (write < MEMSIZE) compacted[write++] = '.';
    memcpy(mem, compacted, MEMSIZE);
}


//Trim leading/trailing spaces
static void trim(char *s) {
    /* trim leading */
    char *p = s;
    while (*p && isspace((unsigned char)*p)) ++p;
    if (p != s) memmove(s, p, strlen(p)+1);
    /* trim trailing */
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len-1])) s[--len] = '\0';
}

// process line logic 
bool process_line(char *line) {
    trim(line);
    if (line[0] == '\0') return true; /* blank */
    if (line[0] == '#') return true;  /* comment */

    char cmd;
    cmd = toupper((unsigned char)line[0]);

    if (cmd == 'A') { 
        // A <name> <size> <algo>
        char name = 0;
        int size = 0;
        char algo = 0;

        char *tok = strtok(line+1, " \t");
        if (!tok) goto badformat;
        name = tok[0];
        tok = strtok(NULL, " \t");

        if (!tok) goto badformat;
        size = atoi(tok);
        tok = strtok(NULL, " \t");

        if (!tok) goto badformat;
        algo = toupper((unsigned char)tok[0]);

        if (!isalpha((unsigned char)name) || !(algo=='F'||algo=='B'||algo=='W')) {
            goto badformat;
        }
        mem_allocate(name, size, algo);
        return true;

    } else if (cmd == 'F') {
        /* F <name> */
        char *tok = strtok(line+1, " \t");

        if (!tok) goto badformat;

        char name = tok[0];
        if (!isalpha((unsigned char)name)) goto badformat;
        mem_free(name);
        return true;

    } else if (cmd == 'S') { // show memory 
        mem_show();
        return true;

    } else if (cmd == 'C') { // compact memory 
        mem_compact();
        return true;

    } else if (cmd == 'R') { // run script from interactive mode 
        char *tok = strtok(line+1, " \t");
        if (!tok) goto badformat;
        return mem_run_script(tok);

    } else if (cmd == 'E') { // end program 
        return false; // signal to exit
    } else {
badformat:
        printf("Error: invalid command or format: \"%s\"\n", line);
        return true;
    }
}

// logic to run the script from interactive mode 
// R <script_name> 
bool mem_run_script(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Error: cannot open script file '%s'\n", filename);
        return false;
    }
    char buf[256];
    while (fgets(buf, sizeof(buf), f)) {
        /* remove trailing newline */
        size_t L = strlen(buf);
        if (L > 0 && (buf[L-1] == '\n' || buf[L-1] == '\r')) buf[--L] = '\0';
        bool cont = process_line(buf);
        if (!cont) { fclose(f); return false; } //E : exit script and signal exit 
    }
    fclose(f);
    return true;
}

