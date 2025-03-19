#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include "system_stats.h"
#include "process.h"
#include "ui.h"

int main() {
    initialize_ui();

    nodelay(stdscr, TRUE); // Make getch non-blocking
    while (1) {
        draw_ui();
        usleep(1000000); // Refresh every 500ms

        int ch = getch();
        if (ch == 'q') {
            break; // Exit loop on 'q'
        }
    }

    endwin(); // Restore terminal state before exit
    return 0;
}
