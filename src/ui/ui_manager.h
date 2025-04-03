/**
 * sysmon - Interactive System Monitor
 * 
 * ui_manager.h - User interface management header
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "../include/sysmon.h"

// Initialize the UI system
bool ui_init(void);

// Update CPU display
void ui_update_cpu(const cpu_metrics_t *metrics);

// Update memory display
void ui_update_memory(const memory_metrics_t *metrics);

// Update network display
void ui_update_network(const network_metrics_t *metrics);

// Update disk display
void ui_update_disk(const disk_metrics_t *metrics);

// Update process display
void ui_update_processes(const process_metrics_t *metrics);

// window resize handler
void ui_handle_resize(void);

// Handle user input
void ui_handle_input(void);

// Refresh the display
void ui_refresh(void);

// Clean up UI resources
void ui_cleanup(void);

#endif /* UI_MANAGER_H */

