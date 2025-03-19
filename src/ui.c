#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include "system_stats.h"
#include "process.h"
#include "ui.h"

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
void draw_ui() {
    clear();

    // Get system stats functions
    double cpu_usage = get_cpu_usage();
    long total_mem, available_mem;
    int running_processes = get_running_processes();

    get_memory_usage(&total_mem, &available_mem);

    // Display system stats
    mvprintw(1, 2, "sysmon - Interactive System Monitor");
    mvprintw(3, 2, "CPU Usage: %.2f%%", cpu_usage);
    mvprintw(4, 2, "Memory Usage: %ld/%ld MB", (total_mem - available_mem) / 1024, total_mem / 1024);
    mvprintw(5, 2, "Running Processes: %d", running_processes);

    // Display active processes
    mvprintw(6, 2, "PID     CPU%%    MEM%%");
    mvprintw(7, 2, "----------------------");

    ProcessInfo processes[MAX_PROCESSES];
    int num_processes = get_active_processes(processes, MAX_PROCESSES);

    for (int i = 0; i < num_processes; i++) {
        mvprintw(8 + i, 2, "%-7d %-7.2f %-7ld", 
                    processes[i].pid, processes[i].cpu_usage, processes[i].mem_usage);

    }

    mvprintw(8 + num_processes, 2, "Press 'q' to quit");
    refresh();
}