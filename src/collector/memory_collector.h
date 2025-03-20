/**
 * sysmon - Interactive System Monitor
 * 
 * memory_collector.h - Memory statistics collector
 */

 #ifndef MEMORY_COLLECTOR_H
 #define MEMORY_COLLECTOR_H
 
 #include "../include/sysmon.h"
 
 // Initialize the memory collector
 bool memory_collector_init(void);
 
 // Collect memory data
 bool memory_collector_collect(memory_data *data);
 
 // Clean up memory collector resources
 void memory_collector_cleanup(void);
 
 #endif /* MEMORY_COLLECTOR_H */