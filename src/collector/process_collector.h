/**
 * sysmon - Interactive System Monitor
 * 
 * process_collector.h - Process statistics collector
 */

#ifndef PROCESS_COLLECTOR_H
#define PROCESS_COLLECTOR_H

#include "../include/sysmon.h"

#define MAX_PROCESSES 1024

// Initialize process collector
bool process_collector_init(void);

// Collect process statistics
bool process_collector_collect(process_metrics_t *metrics);

// Clean up process collector resources
void process_collector_cleanup(void);

#endif /* PROCESS_COLLECTOR_H */
