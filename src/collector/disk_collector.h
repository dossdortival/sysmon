/**
 * sysmon - Interactive System Monitor
 * 
 * disk_collector.h - Disk statistics collector
 */

 #ifndef DISK_COLLECTOR_H
 #define DISK_COLLECTOR_H
 
 #include "../include/sysmon.h"
 
 // Initialize disk collector
 bool disk_collector_init(void);
 
 // Collect disk statistics
 bool disk_collector_collect(disk_metrics_t *metrics);
 
 // Clean up disk collector resources
 void disk_collector_cleanup(void);
 
 #endif /* DISK_COLLECTOR_H */