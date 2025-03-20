/**
 * sysmon - Interactive System Monitor
 * 
 * ui_manager.c - User interface management implementation
 */

 #include <ncurses.h>
 #include <string.h>
 #include <stdlib.h>
 #include <signal.h>
 #include <unistd.h> 
 
 #include "ui_manager.h"
 #include "../util/error_handler.h"
 
 // Windows for different sections
 static WINDOW *header_win = NULL;
 static WINDOW *cpu_win = NULL;
 static WINDOW *memory_win = NULL;
 static WINDOW *footer_win = NULL;
 
 // Screen dimensions
 static int max_y, max_x;
 
 // Display attributes
 static int attr_header;
 static int attr_normal;
 static int attr_highlight;
 static int attr_bar_high;
 static int attr_bar_med;
 static int attr_bar_low;
 
 // Create a horizontal bar
 static void draw_bar(WINDOW *win, int y, int x, int width, double percent, int attr) {
     int bar_width = (int)(width * percent / 100.0);
     
     for (int i = 0; i < width; i++) {
         if (i < bar_width) {
             wattron(win, attr);
             mvwaddch(win, y, x + i, ' ');
             wattroff(win, attr);
         } else {
             mvwaddch(win, y, x + i, ' ');
         }
     }
 }
 
 bool ui_init(void) {
     // Initialize ncurses
     initscr();
     cbreak();
     noecho();
     keypad(stdscr, TRUE);
     curs_set(0);
     timeout(100); // 100ms timeout for getch()
     
     // Check if terminal supports colors
     if (has_colors() == FALSE) {
         endwin();
         log_error("Terminal does not support colors");
         return false;
     }
     
     // Initialize colors
     start_color();
     init_pair(1, COLOR_WHITE, COLOR_BLUE);     // Header
     init_pair(2, COLOR_WHITE, COLOR_BLACK);    // Normal text
     init_pair(3, COLOR_BLACK, COLOR_WHITE);    // Highlighted text
     init_pair(4, COLOR_WHITE, COLOR_RED);      // High usage bar
     init_pair(5, COLOR_WHITE, COLOR_YELLOW);   // Medium usage bar
     init_pair(6, COLOR_WHITE, COLOR_GREEN);    // Low usage bar
     
     // Set up color attributes
     attr_header = COLOR_PAIR(1);
     attr_normal = COLOR_PAIR(2);
     attr_highlight = COLOR_PAIR(3);
     attr_bar_high = COLOR_PAIR(4);
     attr_bar_med = COLOR_PAIR(5);
     attr_bar_low = COLOR_PAIR(6);
     
     // Get screen dimensions
     getmaxyx(stdscr, max_y, max_x);
     
     // Create windows
     header_win = newwin(3, max_x, 0, 0);
     cpu_win = newwin(7, max_x, 3, 0);
     memory_win = newwin(7, max_x, 10, 0);
     footer_win = newwin(3, max_x, max_y - 3, 0);
     
     // Check if windows were created successfully
     if (!header_win || !cpu_win || !memory_win || !footer_win) {
         ui_cleanup();
         log_error("Failed to create ncurses windows");
         return false;
     }
     
     // Draw initial header
     wattron(header_win, attr_header);
     for (int i = 0; i < max_x; i++) {
         mvwaddch(header_win, 0, i, ' ');
         mvwaddch(header_win, 1, i, ' ');
         mvwaddch(header_win, 2, i, ' ');
     }
     mvwprintw(header_win, 1, (max_x - 15) / 2, "sysmon v%s", SYSMON_VERSION);
     wattroff(header_win, attr_header);
     
     // Draw initial footer
     wattron(footer_win, attr_header);
     for (int i = 0; i < max_x; i++) {
         mvwaddch(footer_win, 0, i, ' ');
         mvwaddch(footer_win, 1, i, ' ');
         mvwaddch(footer_win, 2, i, ' ');
     }
     mvwprintw(footer_win, 1, 2, "q: Quit");
     wattroff(footer_win, attr_header);
     
     // Draw initial CPU window
     box(cpu_win, 0, 0);
     mvwprintw(cpu_win, 0, 2, " CPU Usage ");
     
     // Draw initial Memory window
     box(memory_win, 0, 0);
     mvwprintw(memory_win, 0, 2, " Memory Usage ");
     
     // Refresh all windows
     refresh();
     wrefresh(header_win);
     wrefresh(cpu_win);
     wrefresh(memory_win);
     wrefresh(footer_win);
     
     return true;
 }
 
 void ui_update_cpu(const cpu_data *data) {
     if (!data || !cpu_win) return;
     
     // Clear CPU window content
     werase(cpu_win);
     box(cpu_win, 0, 0);
     mvwprintw(cpu_win, 0, 2, " CPU Usage ");
     
     // Display total CPU usage
     mvwprintw(cpu_win, 1, 2, "Total: %5.1f%%", data->total_usage);
     
     // Determine color based on usage
     int bar_attr;
     if (data->total_usage >= 80.0) {
         bar_attr = attr_bar_high;
     } else if (data->total_usage >= 50.0) {
         bar_attr = attr_bar_med;
     } else {
         bar_attr = attr_bar_low;
     }
     
     // Draw total CPU usage bar
     draw_bar(cpu_win, 2, 2, max_x - 4, data->total_usage, bar_attr);
     
     // Display per-core usage
     int cores_per_row = 4;
     int core_width = (max_x - 4) / cores_per_row;
     
     for (int i = 0; i < data->num_cores; i++) {
         int row = i / cores_per_row;
         int col = i % cores_per_row;
         int x_pos = 2 + col * core_width;
         
         if (row > 3) break; // Limit to 4 rows of cores
         
         // Determine color based on core usage
         if (data->core_usage[i] >= 80.0) {
             bar_attr = attr_bar_high;
         } else if (data->core_usage[i] >= 50.0) {
             bar_attr = attr_bar_med;
         } else {
             bar_attr = attr_bar_low;
         }
         
         // Draw core label and percentage
         mvwprintw(cpu_win, 3 + row, x_pos, "CPU%d: %5.1f%%", i, data->core_usage[i]);
         
         // Draw core usage bar
         draw_bar(cpu_win, 3 + row, x_pos + 12, core_width - 13, data->core_usage[i], bar_attr);
     }
 }
 
 void ui_update_memory(const memory_data *data) {
     if (!data || !memory_win) return;
     
     // Convert to MB for display
     double total_mb = data->total / 1024.0;
     double used_mb = data->used / 1024.0;
     double free_mb = data->free / 1024.0;
     double buffers_mb = data->buffers / 1024.0;
     double cached_mb = data->cached / 1024.0;
     double swap_total_mb = data->swap_total / 1024.0;
     double swap_used_mb = data->swap_used / 1024.0;
     
     // Clear Memory window content
     werase(memory_win);
     box(memory_win, 0, 0);
     mvwprintw(memory_win, 0, 2, " Memory Usage ");
     
     // Display memory information
     mvwprintw(memory_win, 1, 2, "Memory: %.1f MB / %.1f MB (%.1f%%)", 
               used_mb, total_mb, data->usage_percent);
     
     // Determine color based on usage
     int bar_attr;
     if (data->usage_percent >= 80.0) {
         bar_attr = attr_bar_high;
     } else if (data->usage_percent >= 50.0) {
         bar_attr = attr_bar_med;
     } else {
         bar_attr = attr_bar_low;
     }
     
     // Draw memory usage bar
     draw_bar(memory_win, 2, 2, max_x - 4, data->usage_percent, bar_attr);
     
     // Display memory details
     mvwprintw(memory_win, 3, 2, "Free: %.1f MB   Buffers: %.1f MB   Cached: %.1f MB", 
               free_mb, buffers_mb, cached_mb);
     
     // Display swap information
     mvwprintw(memory_win, 5, 2, "Swap: %.1f MB / %.1f MB (%.1f%%)",
               swap_used_mb, swap_total_mb, data->swap_usage_percent);
     
     // Determine color for swap bar
     if (data->swap_usage_percent >= 80.0) {
         bar_attr = attr_bar_high;
     } else if (data->swap_usage_percent >= 50.0) {
         bar_attr = attr_bar_med;
     } else {
         bar_attr = attr_bar_low;
     }
     
     // Draw swap usage bar
     draw_bar(memory_win, 6, 2, max_x - 4, data->swap_usage_percent, bar_attr);
 }
 
 void ui_handle_input(void) {
     int ch = getch();
     
     switch (ch) {
         case 'q':
         case 'Q':
             // Set global flag to terminate (handled in main.c)
             kill(getpid(), SIGTERM);
             break;
             
         // Add more keyboard shortcuts as needed
     }
 }
 
 void ui_refresh(void) {
     // Refresh all windows
     wrefresh(header_win);
     wrefresh(cpu_win);
     wrefresh(memory_win);
     wrefresh(footer_win);
 }
 
 void ui_cleanup(void) {
     // Clean up ncurses windows
     if (header_win) delwin(header_win);
     if (cpu_win) delwin(cpu_win);
     if (memory_win) delwin(memory_win);
     if (footer_win) delwin(footer_win);
     
     // End ncurses
     endwin();
 }