/**
 * sysmon - Interactive System Monitor
 * 
 * network_collector.c - Network statistics collector implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>


#include "network_collector.h"
#include "../util/error_handler.h"
 
#define PROC_NET_DEV "/proc/net/dev"
#define MAX_INTERFACE_NAME 16
#define STATS_FILE_LEN 256
 
// Previous readings for rate calculation
typedef struct {
    unsigned long rx_bytes;
    unsigned long tx_bytes;
    time_t last_update;
} interface_prev_t;
 
static interface_prev_t *prev_stats = NULL;
static int num_interfaces = 0;
 
bool network_collector_init(void) {
    // Count number of network interfaces
    FILE *file = fopen(PROC_NET_DEV, "r");
    if (!file) {
        log_error("Failed to open %s", PROC_NET_DEV);
        return false;
    }

    // Skip header lines
    char line[256];
    for (int i = 0; i < 2; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fclose(file);
            log_error("Invalid format in %s", PROC_NET_DEV);
            return false;
        }
    }

    // Count interfaces
    num_interfaces = 0;
    while (fgets(line, sizeof(line), file)) {
        num_interfaces++;
    }
    fclose(file);

    if (num_interfaces == 0) {
        log_warning("No network interfaces found");
        return true;  // Not a fatal error
    }

    // Allocate memory for previous stats
    prev_stats = calloc(num_interfaces, sizeof(interface_prev_t));
    if (!prev_stats) {
        log_error("Memory allocation failed");
        return false;
    }

    // Get initial readings
    network_stats_t dummy;
    return network_collector_collect(&dummy, 1);
}
 
bool network_collector_collect(network_stats_t *stats, int max_interfaces) {
    if (!stats || max_interfaces <= 0) {
        log_error("Invalid arguments");
        return false;
    }

    FILE *file = fopen(PROC_NET_DEV, "r");
    if (!file) {
        log_error("Failed to open %s", PROC_NET_DEV);
        return false;
    }

    // Skip header lines
    char line[256];
    for (int i = 0; i < 2; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fclose(file);
            log_error("Invalid format in %s", PROC_NET_DEV);
            return false;
        }
    }

    time_t now = time(NULL);
    int interface_count = 0;

    while (fgets(line, sizeof(line), file) && interface_count < max_interfaces) {
        char *colon = strchr(line, ':');
        if (!colon) continue;

        // Extract interface name
        char iface[MAX_INTERFACE_NAME];
        strncpy(iface, line, colon - line);
        iface[colon - line] = '\0';
        strcpy(stats[interface_count].interface, iface);

        // Parse statistics
        unsigned long rx_bytes, tx_bytes;
        if (sscanf(colon + 1, "%lu %*u %*u %*u %*u %*u %*u %*u %*u %lu",
                &rx_bytes, &tx_bytes) != 2) {
            log_warning("Failed to parse stats for interface %s", iface);
            continue;
        }

        // Calculate rates
        if (prev_stats[interface_count].last_update > 0) {
            double time_diff = difftime(now, prev_stats[interface_count].last_update);
            if (time_diff > 0) {
                stats[interface_count].rx_rate = 
                    (rx_bytes - prev_stats[interface_count].rx_bytes) / 
                    (time_diff * 1024);  // Convert to KB/s
                stats[interface_count].tx_rate = 
                    (tx_bytes - prev_stats[interface_count].tx_bytes) / 
                    (time_diff * 1024);  // Convert to KB/s
            }
        }

        // Store current values
        stats[interface_count].rx_bytes = rx_bytes;
        stats[interface_count].tx_bytes = tx_bytes;
        prev_stats[interface_count].rx_bytes = rx_bytes;
        prev_stats[interface_count].tx_bytes = tx_bytes;
        prev_stats[interface_count].last_update = now;

        interface_count++;
    }
 
    fclose(file);
    return true;
}
 
void network_collector_cleanup(void) {
    if (prev_stats) {
        free(prev_stats);
        prev_stats = NULL;
    }
    num_interfaces = 0;
}