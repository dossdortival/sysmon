/**
 * sysmon - Interactive System Monitor
 * 
 * sysmon.h - Main header file with common declarations
 */

 #ifndef SYSMON_H
 #define SYSMON_H
 
 #include <stdbool.h>
 
 // Common definitions
 #define MAX_CPU_CORES 64
 #define MAX_PROC_NAME 256
 #define MAX_ERROR_MSG 1024
 #define UI_REFRESH_RATE 1  // In seconds
 
 // CPU data structure
 typedef struct {
     int num_cores;
     double total_usage;
     double core_usage[MAX_CPU_CORES];
 } cpu_data;
 
 // Memory data structure
 typedef struct {
     unsigned long total;
     unsigned long used;
     unsigned long free;
     unsigned long shared;
     unsigned long buffers;
     unsigned long cached;
     unsigned long available;
     double usage_percent;
     
     // Swap
     unsigned long swap_total;
     unsigned long swap_used;
     unsigned long swap_free;
     double swap_usage_percent;
 } memory_data;
 
 // Version information
 #define SYSMON_VERSION "0.1.0"
 
 #endif /* SYSMON_H */