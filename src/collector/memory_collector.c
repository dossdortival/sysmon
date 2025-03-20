/**
 * sysmon - Interactive System Monitor
 * 
 * memory_collector.c - Memory statistics collector implementation
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 #include "memory_collector.h"
 #include "../util/error_handler.h"
 
 bool memory_collector_init(void) {
     // Check if we can access /proc/meminfo
     FILE *file = fopen("/proc/meminfo", "r");
     if (file == NULL) {
         log_error("Failed to open /proc/meminfo for memory initialization");
         return false;
     }
     
     fclose(file);
     return true;
 }
 
 bool memory_collector_collect(memory_data *data) {
     FILE *file = fopen("/proc/meminfo", "r");
     if (file == NULL) {
         log_error("Failed to open /proc/meminfo for memory data collection");
         return false;
     }
     
     char line[256];
     unsigned long mem_total = 0, mem_free = 0, mem_available = 0;
     unsigned long buffers = 0, cached = 0, shmem = 0;
     unsigned long swap_total = 0, swap_free = 0;
     
     while (fgets(line, sizeof(line), file) != NULL) {
         if (strncmp(line, "MemTotal:", 9) == 0) {
             sscanf(line + 9, "%lu", &mem_total);
         } else if (strncmp(line, "MemFree:", 8) == 0) {
             sscanf(line + 8, "%lu", &mem_free);
         } else if (strncmp(line, "MemAvailable:", 13) == 0) {
             sscanf(line + 13, "%lu", &mem_available);
         } else if (strncmp(line, "Buffers:", 8) == 0) {
             sscanf(line + 8, "%lu", &buffers);
         } else if (strncmp(line, "Cached:", 7) == 0) {
             sscanf(line + 7, "%lu", &cached);
         } else if (strncmp(line, "Shmem:", 6) == 0) {
             sscanf(line + 6, "%lu", &shmem);
         } else if (strncmp(line, "SwapTotal:", 10) == 0) {
             sscanf(line + 10, "%lu", &swap_total);
         } else if (strncmp(line, "SwapFree:", 9) == 0) {
             sscanf(line + 9, "%lu", &swap_free);
         }
     }
     
     fclose(file);
     
     // Calculate used memory
     // Used = Total - Free - Buffers - Cached + Shmem
     unsigned long used = mem_total - mem_free - buffers - cached + shmem;
     
     // Fill in the data structure
     data->total = mem_total;
     data->free = mem_free;
     data->available = mem_available;
     data->used = used;
     data->buffers = buffers;
     data->cached = cached;
     data->shared = shmem;
     
     // Calculate usage percentage
     if (mem_total > 0) {
         data->usage_percent = 100.0 * used / mem_total;
     } else {
         data->usage_percent = 0.0;
     }
     
     // Swap information
     data->swap_total = swap_total;
     data->swap_free = swap_free;
     data->swap_used = swap_total - swap_free;
     
     // Calculate swap usage percentage
     if (swap_total > 0) {
         data->swap_usage_percent = 100.0 * (swap_total - swap_free) / swap_total;
     } else {
         data->swap_usage_percent = 0.0;
     }
     
     return true;
 }
 
 void memory_collector_cleanup(void) {
     // Nothing to clean up for the memory collector
 }