/**
 * sysmon - Interactive System Monitor
 * 
 * disk_collector.c - Disk statistics collector implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "disk_collector.h"
#include "../util/error_handler.h"
#include "../util/logger.h"

#define PROC_DISKSTATS "/proc/diskstats"
 
// Static variables for rate calculations
static struct {
    unsigned long prev_read;
    unsigned long prev_write;
    time_t prev_time;
} disk_state;
 
// Initialize disk collector
bool disk_collector_init(void)
{
    memset(&disk_state, 0, sizeof(disk_state));
    disk_state.prev_time = time(NULL);
    return true;
}
 
// Read global disk stats from /proc/diskstats
static bool read_global_disk_stats(disk_metrics_t *metrics)
{
    FILE *fp = fopen(PROC_DISKSTATS, "r");
    if (!fp) {
        log_error("Failed to open %S", PROC_DISKSTATS);
        return false;
    }

    char line[256];
    unsigned long total_read = 0;
    unsigned long total_write = 0;
    time_t current_time = time(NULL);
    double time_diff = difftime(current_time, disk_state.prev_time);
    
    if (time_diff <= 0) {
        fclose(fp);
        return false;
    }

    // Sum up all disk I/O
    while (fgets(line, sizeof(line), fp)) {
        unsigned long reads, writes;
        
        // Format: major minor name reads sectors_read writes sectors_written
        if (sscanf(line, "%*d %*d %*s %lu %*lu %lu %*lu", &reads, &writes) == 2) {
            total_read += reads;
            total_write += writes;
        }
    }
    fclose(fp);

    // Calculate rates (sectors are typically 512 bytes)
    metrics->read_rate = ((total_read - disk_state.prev_read) * 512) / (time_diff * 1024);
    metrics->write_rate = ((total_write - disk_state.prev_write) * 512) / (time_diff * 1024);
    
    // Update totals (convert sectors to KB)
    metrics->total_read += (total_read * 512) / 1024;
    metrics->total_written += (total_write * 512) / 1024;

    // Save current values for next calculation
    disk_state.prev_read = total_read;
    disk_state.prev_write = total_write;
    disk_state.prev_time = current_time;

    return true;
}
 
// Collect disk statistics
bool disk_collector_collect(disk_metrics_t *metrics)
{
    if (!metrics) return false;

    // Initialize metrics
    memset(metrics, 0, sizeof(disk_metrics_t));

    // Get disk I/O statistics
    if (!read_global_disk_stats(metrics)) {
        log_warning("Could not collect disk I/O stats");
        return false;
    }

    return true;
}
 
// Clean up disk collector resources
void disk_collector_cleanup(void)
{
    // No special cleanup needed
}