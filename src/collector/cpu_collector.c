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

// File containing CPU statistics
#define PROC_STAT_PATH "/proc/stat"

// Previous CPU time measurements
static struct {
    unsigned long user[MAX_CPU_CORES + 1];   // +1 for total CPU
    unsigned long nice[MAX_CPU_CORES + 1];
    unsigned long system[MAX_CPU_CORES + 1];
    unsigned long idle[MAX_CPU_CORES + 1];
    unsigned long iowait[MAX_CPU_CORES + 1];
    unsigned long irq[MAX_CPU_CORES + 1];
    unsigned long softirq[MAX_CPU_CORES + 1];
    unsigned long steal[MAX_CPU_CORES + 1];
} prev_times;

static int num_cores = 0;

bool cpu_collector_init(void) {
    FILE *file = fopen(PROC_STAT_PATH, "r");
    if (file == NULL) {
        log_error("Failed to open %s", PROC_STAT_PATH);
        return false;
    }

    // Count the number of CPU cores
    char line[256];
    num_cores = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "cpu", 3) == 0 && isdigit(line[3])) {
            num_cores++;
        }
    }
    
    fclose(file);

    if (num_cores == 0) {
        log_error("No CPU cores detected");
        return false;
    }

    if (num_cores > MAX_CPU_CORES) {
        log_warning("Truncating detected cores from %d to %d", num_cores, MAX_CPU_CORES);
        num_cores = MAX_CPU_CORES;
    }

    // Initialize with first reading
    return cpu_collector_collect(NULL);
}

bool cpu_collector_collect(cpu_data *data) {
    if (data == NULL) {
        // Initialization call - just populate previous values
        cpu_data dummy;
        return cpu_collector_collect(&dummy);
    }

    FILE *file = fopen(PROC_STAT_PATH, "r");
    if (file == NULL) {
        log_error("Failed to open %s", PROC_STAT_PATH);
        return false;
    }

    char line[256];
    int core_index = -1;  // -1 for total CPU, 0+ for cores
    data->num_cores = num_cores;

    while (fgets(line, sizeof(line), file) && core_index < num_cores) {
        if (strncmp(line, "cpu", 3) != 0) continue;

        // Determine if this is total CPU or specific core
        if (isdigit(line[3])) {
            core_index = atoi(line + 3);
            if (core_index >= num_cores) continue;
        } else {
            core_index = -1;  // Total CPU
        }

        // Parse CPU time values
        unsigned long user, nice, system, idle, iowait, irq, softirq, steal;
        if (sscanf(line + 5, "%lu %lu %lu %lu %lu %lu %lu %lu",
                  &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) < 4) {
            continue;
        }

        // Calculate usage percentages
        int store_index = core_index + 1;  // +1 because total CPU is at 0
        unsigned long total = user + nice + system + idle + iowait + irq + softirq + steal;
        unsigned long prev_total = prev_times.user[store_index] + prev_times.nice[store_index] +
                                 prev_times.system[store_index] + prev_times.idle[store_index] +
                                 prev_times.iowait[store_index] + prev_times.irq[store_index] +
                                 prev_times.softirq[store_index] + prev_times.steal[store_index];

        if (prev_total > 0 && total > prev_total) {
            unsigned long diff_total = total - prev_total;
            unsigned long diff_used = diff_total - (idle - prev_times.idle[store_index]);

            double usage = 100.0 * diff_used / diff_total;

            if (core_index == -1) {
                data->total_usage = usage;
            } else {
                data->core_usage[core_index] = usage;
            }
        }

        // Store current values for next reading
        prev_times.user[store_index] = user;
        prev_times.nice[store_index] = nice;
        prev_times.system[store_index] = system;
        prev_times.idle[store_index] = idle;
        prev_times.iowait[store_index] = iowait;
        prev_times.irq[store_index] = irq;
        prev_times.softirq[store_index] = softirq;
        prev_times.steal[store_index] = steal;
    }

    fclose(file);
    return true;
}

void cpu_collector_cleanup(void) {
    // No dynamic resources to clean up
}
