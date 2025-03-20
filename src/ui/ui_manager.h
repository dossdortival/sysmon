/**
 * sysmon - Interactive System Monitor
 * 
 * ui_manager.h - User interface management
 */

 #ifndef UI_MANAGER_H
 #define UI_MANAGER_H
 
 #include "../include/sysmon.h"
 
 // Initialize the UI system
 bool ui_init(void);
 
 // Update CPU display
 void ui_update_cpu(const cpu_data *data);
 
 // Update memory display
 void ui_update_memory(const memory_data *data);
 
 // Handle user input
 void ui_handle_input(void);
 
 // Refresh the display
 void ui_refresh(void);
 
 // Clean up UI resources
 void ui_cleanup(void);
 
 #endif /* UI_MANAGER_H */