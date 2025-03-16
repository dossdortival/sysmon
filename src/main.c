#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include "system_stats.h"

// Initialize ncurses UI
void initialize_ui() {
    if (initscr() == NULL) {
        fprintf(stderr, "Error initializing ncurses.\n");
        exit(EXIT_FAILURE);
    }
    noecho();          // Disable echoing of typed characters
    curs_set(FALSE);   // Hide cursor
    refresh();
}

// Update UI with system stats
void update_ui() {
    clear();
    
    double cpu_usage = get_cpu_usage();
    long total_mem, available_mem;
    get_memory_usage(&total_mem, &available_mem);

    mvprintw(1, 2, "sysmon - Interactive System Monitor");
    mvprintw(3, 2, "CPU Usage: %.2f%%", cpu_usage);
    mvprintw(4, 2, "Memory Usage: %ld/%ld MB", (total_mem - available_mem) / 1024, total_mem / 1024);
    mvprintw(5, 2, "Press 'q' to quit");
    
    refresh();
}

int main() {
    initialize_ui();

    while (1) {
        update_ui();
        usleep(500000); // Refresh every 500ms

        int ch = getch();
        if (ch == 'q') {
            break; // Exit loop on 'q'
        }
    }

    endwin(); // Restore terminal state before exit
    return 0;
}