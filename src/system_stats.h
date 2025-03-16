#ifndef SYSTEM_STATS_H
#define SYSTEM_STATS_H

// system_stats.h - Header file for system resource monitoring functions

// Function to get CPU usage percentage
double get_cpu_usage();

// Function to get total and free memory (in KB)
void get_memory_usage(long *total_mem, long *available_mem);

#endif // SYSTEM_STATS_H