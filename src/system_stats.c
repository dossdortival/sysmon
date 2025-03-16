#include "system_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function to get CPU usage percentage
double get_cpu_usage() {
    static long prev_idle = 0, prev_total = 0; // Static variables to retain values
    FILE *file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Failed to open /proc/stat");
        return -1.0;
    }

    char line[256];
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    // Read the first line of /proc/stat
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return -1.0;
    }

    sscanf(line, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    fclose(file);

    // Calculate total and idle CPU time
    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    long total_idle = idle + iowait;

    // Calculate CPU usage percentage
    long delta_total = total - prev_total;
    long delta_idle = total_idle - prev_idle;

    // Prevent division by zero
    double cpu_usage = (delta_total > 0) ? (100.0 * (delta_total - delta_idle) / delta_total) : 0.0;

    // Update previous values for next iteration
    prev_total = total;
    prev_idle = total_idle;

    return cpu_usage;
}

// Function to get total and available memory (in KB)
void get_memory_usage(long *total_mem, long *available_mem) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) {
        perror("Failed to open /proc/meminfo");
        return;
    }

    char line[256];
    *total_mem = 0;
    *available_mem = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "MemTotal: %ld kB", total_mem) == 1) {
            continue;
        }
        if (sscanf(line, "MemAvailable: %ld kB", available_mem) == 1) {
            break;
        }
    }

    fclose(file);
}
