/**
 * sysmon - Interactive System Monitor
 * 
 * network_collector.h - Network statistics collector
 */

#ifndef NETWORK_COLLECTOR_H
#define NETWORK_COLLECTOR_H

#include "../include/sysmon.h"
 
// Initialize network collector 
bool network_collector_init(void);

// Collect network statistics
bool network_collector_collect(network_metrics_t *metrics);

// Clean up network collector resources
void network_collector_cleanup(void);

#endif /* NETWORK_COLLECTOR_H */