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

// Window layout configuration
typedef struct {
    WINDOW *win;
    int height;
    int y_pos;
} window_layout_t;

// UI color attributes
typedef struct {
    int header;
    int normal;
    int highlight;
    int bar_high;
    int bar_medium;
    int bar_low;
} ui_attributes_t;

// UI component dimensions
typedef struct {
    int max_y;
    int max_x;
    int bar_width;
    int cores_per_row;
} ui_dimensions_t;

// Static UI state
static struct {
    window_layout_t header;
    window_layout_t cpu;
    window_layout_t memory;
    window_layout_t network;
    window_layout_t disk;
    window_layout_t processes;
    window_layout_t footer;
    ui_attributes_t attr;
    ui_dimensions_t dim;
} ui;

// Draw a horizontal progress bar
static void draw_progress_bar(WINDOW *win, int y, int x, double percent, int attr) 
{
    int fill_width = (int)(ui.dim.bar_width * percent / 100.0);
    fill_width = (fill_width > ui.dim.bar_width) ? ui.dim.bar_width : fill_width;

    wattron(win, attr);
    for (int i = 0; i < fill_width; i++) {
        mvwaddch(win, y, x + i, ' ');
    }
    wattroff(win, attr);
}

// Get color attribute based on usage percentage
static int get_usage_color(double percent) 
{
    if (percent >= 80.0) return ui.attr.bar_high;
    if (percent >= 50.0) return ui.attr.bar_medium;
    return ui.attr.bar_low;
}

// Initialize color attributes
static bool init_colors(void) 
{
    if (!has_colors()) {
        log_error("Terminal does not support colors");
        return false;
    }

    start_color();
    use_default_colors();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);    // Header/footer
    init_pair(2, COLOR_WHITE, COLOR_BLACK);   // Normal text
    init_pair(3, COLOR_BLACK, COLOR_WHITE);   // Highlighted text
    init_pair(4, COLOR_WHITE, COLOR_RED);     // High usage
    init_pair(5, COLOR_WHITE, COLOR_YELLOW);  // Medium usage
    init_pair(6, COLOR_WHITE, COLOR_GREEN);   // Low usage

    ui.attr.header = COLOR_PAIR(1);
    ui.attr.normal = COLOR_PAIR(2);
    ui.attr.highlight = COLOR_PAIR(3);
    ui.attr.bar_high = COLOR_PAIR(4);
    ui.attr.bar_medium = COLOR_PAIR(5);
    ui.attr.bar_low = COLOR_PAIR(6);

    return true;
}

// Create and initialize a window
static bool init_window(window_layout_t *win, int height, int y_pos, const char *title) 
{
    win->height = height;
    win->y_pos = y_pos;
    win->win = newwin(height, ui.dim.max_x, y_pos, 0);

    if (!win->win) {
        log_error("Failed to create window");
        return false;
    }

    box(win->win, 0, 0);
    if (title) {
        mvwprintw(win->win, 0, 2, " %s ", title);
    }
    return true;
}

// Initialize the UI system
bool ui_init(void) 
{
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(100);  // 100ms input timeout

    if (!init_colors()) {
        endwin();
        return false;
    }

    // Get screen dimensions
    getmaxyx(stdscr, ui.dim.max_y, ui.dim.max_x);
    ui.dim.bar_width = ui.dim.max_x - 4;
    ui.dim.cores_per_row = 4;

    // Initialize windows
    if (!init_window(&ui.header, 3, 0, NULL) ||
        !init_window(&ui.cpu, 7, 3, "CPU Usage") ||
        !init_window(&ui.memory, 7, 10, "Memory Usage") ||
        !init_window(&ui.network, 7, 17, "Network Activity") ||
        !init_window(&ui.disk, 7, 24, "Disk I/O") ||
        !init_window(&ui.processes, ui.dim.max_y - 13, 31, "Processes") || 
        !init_window(&ui.footer, 3, ui.dim.max_y - 3, NULL)) {
        ui_cleanup();
        return false;
    }

    // Draw header
    wattron(ui.header.win, ui.attr.header);
    mvwprintw(ui.header.win, 1, (ui.dim.max_x - 15) / 2, "sysmon v%s", SYSMON_VERSION);
    wattroff(ui.header.win, ui.attr.header);

    // Draw footer
    wattron(ui.footer.win, ui.attr.header);
    mvwprintw(ui.footer.win, 1, 2, "q: Quit");
    wattroff(ui.footer.win, ui.attr.header);

    ui_refresh();
    return true;
}

// Update CPU metrics display
void ui_update_cpu(const cpu_metrics_t *metrics) 
{
    if (!metrics || !ui.cpu.win) return;

    werase(ui.cpu.win);
    box(ui.cpu.win, 0, 0);
    mvwprintw(ui.cpu.win, 0, 2, " CPU Usage ");

    // Display total CPU usage
    mvwprintw(ui.cpu.win, 1, 2, "Total: %5.1f%%", metrics->total_usage);
    draw_progress_bar(ui.cpu.win, 2, 2, metrics->total_usage, 
                     get_usage_color(metrics->total_usage));

    // Display per-core usage
    int core_width = ui.dim.bar_width / ui.dim.cores_per_row;
    for (int i = 0; i < metrics->num_cores && i < MAX_CPU_CORES; i++) {
        int row = i / ui.dim.cores_per_row;
        int col = i % ui.dim.cores_per_row;
        int x_pos = 2 + col * core_width;

        if (row > 3) break;  // Limit to 4 rows

        mvwprintw(ui.cpu.win, 3 + row, x_pos, "CPU%d: %5.1f%%", 
                 i, metrics->core_usage[i]);
        draw_progress_bar(ui.cpu.win, 3 + row, x_pos + 12, metrics->core_usage[i],
                         get_usage_color(metrics->core_usage[i]));
    }
}

// Update memory metrics display
void ui_update_memory(const memory_metrics_t *metrics) 
{
    if (!metrics || !ui.memory.win) return;

    werase(ui.memory.win);
    box(ui.memory.win, 0, 0);
    mvwprintw(ui.memory.win, 0, 2, " Memory Usage ");

    // Convert to MB for display
    double total_mb = metrics->total / 1024.0;
    double used_mb = metrics->used / 1024.0;

    // Display memory usage
    mvwprintw(ui.memory.win, 1, 2, "Memory: %.1f MB / %.1f MB (%.1f%%)", 
             used_mb, total_mb, metrics->usage_percent);
    draw_progress_bar(ui.memory.win, 2, 2, metrics->usage_percent,
                     get_usage_color(metrics->usage_percent));

    // Display memory details
    mvwprintw(ui.memory.win, 3, 2, "Free: %.1f MB   Buffers: %.1f MB   Cached: %.1f MB",
             metrics->free / 1024.0, metrics->buffers / 1024.0, metrics->cached / 1024.0);

    // Display swap usage
    double swap_total_mb = metrics->swap_total / 1024.0;
    double swap_used_mb = metrics->swap_used / 1024.0;

    mvwprintw(ui.memory.win, 5, 2, "Swap: %.1f MB / %.1f MB (%.1f%%)",
             swap_used_mb, swap_total_mb, metrics->swap_usage_percent);
    draw_progress_bar(ui.memory.win, 6, 2, metrics->swap_usage_percent,
                     get_usage_color(metrics->swap_usage_percent));
}

// Update network metrics display
void ui_update_network(const network_metrics_t *metrics) 
{
    if (!metrics || !ui.network.win) return;

    werase(ui.network.win);
    box(ui.network.win, 0, 0);
    mvwprintw(ui.network.win, 0, 2, " Network Usage ");

    // Display primary interface stats
    mvwprintw(ui.network.win, 1, 2, "Interface: %-10s", metrics->interface);
    
    // Display transfer rates
    mvwprintw(ui.network.win, 2, 2, "Download: %6.1f KB/s", metrics->rx_rate);
    mvwprintw(ui.network.win, 3, 2, "Upload:   %6.1f KB/s", metrics->tx_rate);

    // Display totals (convert to MB)
    mvwprintw(ui.network.win, 5, 2, "Total: ↓%-6.1f MB ↑%-6.1f MB", 
             metrics->total_rx/1024.0, metrics->total_tx/1024.0);
}

// Update disk metrics display
void ui_update_disk(const disk_metrics_t *metrics)
{
    if (!metrics || !ui.disk.win) return;

    werase(ui.disk.win);
    box(ui.disk.win, 0, 0);
    mvwprintw(ui.disk.win, 0, 2, " Disk I/O ");

    // Display read stats
    mvwprintw(ui.disk.win, 1, 2, "Read: %6.1f KB/s", metrics->read_rate);
    draw_progress_bar(ui.disk.win, 2, 2, 
                    metrics->read_rate / 10.0,  // Scale to 1000KB/s = 100%
                    get_usage_color(metrics->read_rate / 10.0));

    // Display write stats
    mvwprintw(ui.disk.win, 3, 2, "Write: %6.1f KB/s", metrics->write_rate);
    draw_progress_bar(ui.disk.win, 4, 2,
                    metrics->write_rate / 10.0,  // Scale to 1000KB/s = 100%
                    get_usage_color(metrics->write_rate / 10.0));

    // Display totals
    mvwprintw(ui.disk.win, 5, 2, "Total Read: %.1f MB", metrics->total_read / 1024.0);
    mvwprintw(ui.disk.win, 6, 2, "Total Written: %.1f MB", metrics->total_written / 1024.0);
}

// Update process metrics display
void ui_update_processes(const process_metrics_t *metrics)
{
    if (!metrics || !ui.processes.win || metrics->count <= 0) return;

    werase(ui.processes.win);
    box(ui.processes.win, 0, 0);
    mvwprintw(ui.processes.win, 0, 2, " Processes ");

    // Display header
    mvwprintw(ui.processes.win, 1, 2, "%-6s %6s %6s %s", 
             "PID", "CPU%", "MEM%", "NAME");

    // Display processes
    int max_rows = ui.processes.height - 3;
    int to_show = metrics->count > max_rows ? max_rows : metrics->count;

    for (int i = 0; i < to_show; i++) {
        const process_info_t *p = &metrics->processes[i];
        mvwprintw(ui.processes.win, 2+i, 2, "%-6d %6.1f %6.1f %s",
                 p->pid, p->cpu_usage, p->mem_usage, p->name);
    } 
}
 
// Handle user input
void ui_handle_input(void) 
{
    int ch = getch();
    if (ch == 'q' || ch == 'Q') {
        kill(getpid(), SIGTERM);
    }
}

// Refresh the display
void ui_refresh(void) 
{
    wrefresh(ui.header.win);
    wrefresh(ui.cpu.win);
    wrefresh(ui.memory.win);
    wrefresh(ui.network.win);
    wrefresh(ui.disk.win);
    wrefresh(ui.processes.win);
    wrefresh(ui.footer.win);
    refresh();
}

// Clean up UI resources
void ui_cleanup(void) 
{
    if (ui.header.win) delwin(ui.header.win);
    if (ui.cpu.win) delwin(ui.cpu.win);
    if (ui.memory.win) delwin(ui.memory.win);
    if (ui.network.win) delwin(ui.network.win);
    if (ui.disk.win) delwin(ui.disk.win);
    if (ui.processes.win) delwin(ui.processes.win);
    if (ui.footer.win) delwin(ui.footer.win);
    endwin();
}