/**
 * sysmon - Interactive System Monitor
 * 
 * cpu_collector.c - CPU statistics collector implementation
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <ctype.h>
 
 #include "cpu_collector.h"
 #include "../util/error_handler.h"
 
 // Static variables to store previous measurements
 static unsigned long prev_total_user[MAX_CPU_CORES + 1] = {0};
 static unsigned long prev_total_user_low[MAX_CPU_CORES + 1] = {0};
 static unsigned long prev_total_sys[MAX_CPU_CORES + 1] = {0};
 static unsigned long prev_total_idle[MAX_CPU_CORES + 1] = {0};
 static int num_cores = 0;
 
 bool cpu_collector_init(void) {
     FILE *file = fopen("/proc/stat", "r");
     if (file == NULL) {
         log_error("Failed to open /proc/stat for CPU initialization");
         return false;
     }
     
     char line[256];
     int core_count = 0;
     
     while (fgets(line, sizeof(line), file) != NULL) {
         if (strncmp(line, "cpu", 3) == 0 && isdigit(line[3])) {
             core_count++;
         }
     }
     
     fclose(file);
     
     if (core_count == 0) {
         log_error("No CPU cores detected");
         return false;
     }
     
     if (core_count > MAX_CPU_CORES) {
         log_warning("Detected %d CPU cores, but only %d supported", core_count, MAX_CPU_CORES);
         core_count = MAX_CPU_CORES;
     }
     
     num_cores = core_count;
     
     // Initialize with first reading
     cpu_data dummy;
     return cpu_collector_collect(&dummy);
 }
 
 bool cpu_collector_collect(cpu_data *data) {
     FILE *file = fopen("/proc/stat", "r");
     if (file == NULL) {
         log_error("Failed to open /proc/stat for CPU data collection");
         return false;
     }
     
     char line[256];
     data->num_cores = num_cores;
     
     // Read CPU statistics for each core and total
     int core_index = -1; // Start with -1 for total CPU
     
     while (fgets(line, sizeof(line), file) != NULL && core_index < num_cores) {
         // Check if this is a CPU line
         if (strncmp(line, "cpu", 3) == 0) {
             char *cpu_str = line + 3;
             
             // If it's the total CPU line (no digit after "cpu")
             if (!isdigit(*cpu_str)) {
                 core_index = -1; // Special index for total CPU
             } else {
                 // Parse the core number
                 core_index = atoi(cpu_str);
                 if (core_index >= num_cores) {
                     continue; // Skip cores beyond our limit
                 }
             }
             
             // Parse the CPU time values
             unsigned long user, nice, system, idle, iowait, irq, softirq, steal;
             if (sscanf(line + 5, "%lu %lu %lu %lu %lu %lu %lu %lu",
                        &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) < 4) {
                 continue; // Skip if we can't read the minimum required values
             }
             
             // Calculate total values
             unsigned long total_user = user + nice;
             unsigned long total_sys = system + irq + softirq;
             unsigned long total_idle = idle + iowait;
             unsigned long total = total_user + total_sys + total_idle + steal;
             printf("Total CPU time: %lu\n", total); // **Debugging**
             
             // Calculate actual CPU usage
             if (core_index >= 0) {
                 // For individual cores
                 unsigned long total_user_diff = total_user - prev_total_user[core_index + 1];
                 unsigned long total_sys_diff = total_sys - prev_total_sys[core_index + 1];
                 unsigned long total_idle_diff = total_idle - prev_total_idle[core_index + 1];
                 unsigned long total_diff = total_user_diff + total_sys_diff + total_idle_diff + 
                                          (steal - prev_total_user_low[core_index + 1]);
                 
                 if (total_diff > 0) {
                     data->core_usage[core_index] = 100.0 * (total_diff - total_idle_diff) / total_diff;
                 } else {
                     data->core_usage[core_index] = 0.0;
                 }
                 
                 // Store current values for next reading
                 prev_total_user[core_index + 1] = total_user;
                 prev_total_sys[core_index + 1] = total_sys;
                 prev_total_idle[core_index + 1] = total_idle;
                 prev_total_user_low[core_index + 1] = steal;
             } else {
                 // For total CPU
                 unsigned long total_user_diff = total_user - prev_total_user[0];
                 unsigned long total_sys_diff = total_sys - prev_total_sys[0];
                 unsigned long total_idle_diff = total_idle - prev_total_idle[0];
                 unsigned long total_diff = total_user_diff + total_sys_diff + total_idle_diff + 
                                          (steal - prev_total_user_low[0]);
                 
                 if (total_diff > 0) {
                     data->total_usage = 100.0 * (total_diff - total_idle_diff) / total_diff;
                 } else {
                     data->total_usage = 0.0;
                 }
                 
                 // Store current values for next reading
                 prev_total_user[0] = total_user;
                 prev_total_sys[0] = total_sys;
                 prev_total_idle[0] = total_idle;
                 prev_total_user_low[0] = steal;
             }
         }
     }
     
     fclose(file);
     return true;
 }
 
 void cpu_collector_cleanup(void) {
     // Nothing to clean up for the CPU collector
 }