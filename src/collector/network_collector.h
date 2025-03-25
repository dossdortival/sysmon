/**
 * sysmon - Interactive System Monitor
 * 
 * network_collector.h - Network statistics collector
 */

#ifndef NETWORK_COLLECTOR_H
#define NETWORK_COLLECTOR_H

#include "../include/sysmon.h"
 
// Network interface statistics structure
typedef struct {
    char interface[16];          // Interface name (eth0, wlan0, etc.)
    unsigned long rx_bytes;      // Total received bytes
    unsigned long tx_bytes;      // Total transmitted bytes
    double rx_rate;              // Receive rate (KB/s)
    double tx_rate;              // Transmit rate (KB/s)
} network_stats_t;
 
// Initialize network collector
bool network_collector_init(void);

// Collect network statistics
bool network_collector_collect(network_stats_t *stats, int max_interfaces);

// Clean up network collector resources
void network_collector_cleanup(void);

#endif /* NETWORK_COLLECTOR_H */