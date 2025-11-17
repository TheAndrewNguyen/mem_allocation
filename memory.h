#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdbool.h>

#define MEMSIZE 80

// initalize memory pool
// fills in memory with . 
void mem_init(void);

// display memory 
void mem_show(void);

// allocate with algorithm 
bool mem_allocate(char name, int size, char algo);

// free memory 
void mem_free(char name);

// compact memory pool 
void mem_compact(void);

// file mode 
bool mem_run_script(const char *filename);

// calls init memory 
void mem_clear(void);

#endif /* MEMORY_H */
