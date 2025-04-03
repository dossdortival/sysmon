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
#include <ctype.h>
#include <limits.h>
 
#include "network_collector.h"
#include "../util/error_handler.h"

#define PROC_NET_DEV "/proc/net/dev"
#define MAX_INTERFACE_NAME 16
#define STATS_FILE_LEN 256

// Previous readings for rate calculation
typedef struct {
    unsigned long rx_bytes; // Received bytes
    unsigned long tx_bytes; // Transmitted bytes
    time_t last_update; 
} interface_prev_t;

static interface_prev_t *prev_stats = NULL;
static int num_interfaces = 0;

// Helper to trim whitespace from interface name
static void trim_whitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    *(end+1) = '\0';
}

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
    network_metrics_t dummy;
    return network_collector_collect(&dummy);
}

bool network_collector_collect(network_metrics_t *metrics) {
    if (!metrics) return false;

    FILE *fp = fopen(PROC_NET_DEV, "r");
    if (!fp) {
        log_error("Failed to open network stats");
        return false;
    }

    memset(metrics, 0, sizeof(network_metrics_t));
    time_t now = time(NULL);
    double time_diff = 1.0; // Default to 1s if no previous data

    // Calculate time difference if we have previous data
    if (prev_stats[0].last_update > 0) {
        time_diff = difftime(now, prev_stats[0].last_update);
        if (time_diff <= 0) time_diff = 1.0; // Prevent division by zero
    }

    // Skip header lines
    char line[256];
    for (int i = 0; i < 2; i++) fgets(line, sizeof(line), fp);

    unsigned long max_rx = 0;
    char primary_iface[MAX_INTERFACE_NAME] = "";

    while (fgets(line, sizeof(line), fp)) {
        char *colon = strchr(line, ':');
        if (!colon) continue;

        // Extract interface name
        char iface[MAX_INTERFACE_NAME];
        strncpy(iface, line, colon - line);
        iface[colon - line] = '\0';
        trim_whitespace(iface);

        // Skip loopback unless it's the only interface
        if (strcmp(iface, "lo") == 0) continue;

        // Parse statistics
        unsigned long rx_bytes, tx_bytes;
        if (sscanf(colon+1, "%lu %*u %*u %*u %*u %*u %*u %*u %lu",
                  &rx_bytes, &tx_bytes) != 2) {
            continue;
        }

        // Track interface with most traffic
        if (rx_bytes > max_rx) {
            max_rx = rx_bytes;
            strncpy(primary_iface, iface, MAX_INTERFACE_NAME);
            
            // Only calculate rates if we have previous data
            if (prev_stats[0].last_update > 0) {
                // Handle counter wrap-around (32-bit systems)
                if (rx_bytes < prev_stats[0].rx_bytes) {
                    metrics->rx_rate = ((ULONG_MAX - prev_stats[0].rx_bytes) + rx_bytes) / (time_diff * 1024);
                } else {
                    metrics->rx_rate = (rx_bytes - prev_stats[0].rx_bytes) / (time_diff * 1024);
                }

                if (tx_bytes < prev_stats[0].tx_bytes) {
                    metrics->tx_rate = ((ULONG_MAX - prev_stats[0].tx_bytes) + tx_bytes) / (time_diff * 1024);
                } else {
                    metrics->tx_rate = (tx_bytes - prev_stats[0].tx_bytes) / (time_diff * 1024);
                }
            }

            strncpy(metrics->interface, iface, MAX_INTERFACE_NAME);
            metrics->rx_bytes = rx_bytes;
            metrics->tx_bytes = tx_bytes;
            metrics->total_rx = rx_bytes / 1024;  // KB
            metrics->total_tx = tx_bytes / 1024;  // KB
        }
    }
    fclose(fp);

    // Update previous stats
    if (primary_iface[0] != '\0') {
        prev_stats[0].rx_bytes = metrics->rx_bytes;
        prev_stats[0].tx_bytes = metrics->tx_bytes;
        prev_stats[0].last_update = now;
    }
    // Fallback to loopback if no other interfaces
    else if (num_interfaces == 1) {
        strncpy(metrics->interface, "lo", MAX_INTERFACE_NAME);
    }

    return true;
}

void network_collector_cleanup(void) {
    if (prev_stats) {
        free(prev_stats);
        prev_stats = NULL;
    }
    num_interfaces = 0;
}