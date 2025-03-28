/**
 * sysmon - Interactive System Monitor
 * 
 * cpu_collector.h - CPU statistics collector
 */

#ifndef CPU_COLLECTOR_H
#define CPU_COLLECTOR_H

#include "../include/sysmon.h"

// Maximum number of CPU cores we support tracking
#define MAX_CPU_CORES 64

// Structure to hold CPU usage data
typedef struct {
    double total_usage;          // Total CPU usage percentage
    double core_usage[MAX_CPU_CORES]; // Per-core usage percentages
    int num_cores;               // Number of CPU cores detected
} cpu_data;

// Initialize the CPU collector
bool cpu_collector_init(void);

// Collect CPU data
bool cpu_collector_collect(cpu_data *data);

// Clean up CPU collector resources
void cpu_collector_cleanup(void);

#endif /* CPU_COLLECTOR_H */
