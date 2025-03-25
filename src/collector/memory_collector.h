/**
 * sysmon - Interactive System Monitor
 * 
 * memory_collector.h - Memory statistics collector
 */

#ifndef MEMORY_COLLECTOR_H
#define MEMORY_COLLECTOR_H

#include "../include/sysmon.h"

// Structure to hold memory usage data
typedef struct {
    unsigned long total;          // Total physical memory (KB)
    unsigned long free;           // Free memory (KB)
    unsigned long available;      // Available memory (KB)
    unsigned long used;           // Used memory (KB)
    unsigned long buffers;        // Buffers memory (KB)
    unsigned long cached;         // Cached memory (KB)
    unsigned long shared;         // Shared memory (KB)
    double usage_percent;         // Memory usage percentage
    unsigned long swap_total;     // Total swap space (KB)
    unsigned long swap_free;      // Free swap space (KB)
    unsigned long swap_used;      // Used swap space (KB)
    double swap_usage_percent;    // Swap usage percentage
} memory_data;
 
// Initialize the memory collector
bool memory_collector_init(void);

// Collect memory data
bool memory_collector_collect(memory_data *data);

// Clean up memory collector resources
void memory_collector_cleanup(void);

#endif /* MEMORY_COLLECTOR_H */