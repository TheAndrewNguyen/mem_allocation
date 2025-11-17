#include "memory.h"
#include <stdio.h>
#include <unistd.h>  // for isatty() and fileno()

bool process_line(char *line);
bool mem_run_script(const char *file); // run commands from a file

int main(void) {
    mem_init();

    char line[256];

    // interactive mode 
    if (isatty(fileno(stdin))) {
        // motd 
        printf("Running in Interactive Mode: \n"); 
        printf("Contiguous Memory Allocation (MEMSIZE=%d)\n", MEMSIZE);
        printf("Commands:\n");
        printf("  A <name> <size> <algo(F/B/W)>  Allocate memory\n");
        printf("  F <name>                       Free memory\n");
        printf("  S                              Show memory\n");
        printf("  C                              Compact memory\n");
        printf("  R <file>                       Run commands from file\n");
        printf("  E                              Exit\n");

        while (1) {
            printf("> ");
            // user input 
            if (!fgets(line, sizeof(line), stdin)) {
                putchar('\n');
                break;
            }
            
            // process each command 
            if(!(process_line(line))) {
                break;
            }
        }
    } else { // script mode, no motd or > 
        printf("Running in Script Mode \n "); 
        while (1) {
            if (!fgets(line, sizeof(line), stdin)) {
                putchar('\n');
                break;
            }
            
            // process each command 
            if(!(process_line(line))) {
                break;
            }
        }
    }

    printf("Exiting.\n");
    return 0;
}
