/**
 * sysmon - Interactive System Monitor
 * 
 * sysmon.h - Main header file with common declarations
 */

#ifndef SYSMON_H
#define SYSMON_H

#include <stdbool.h>
#include <sys/types.h>

// =============================================
// System Configuration Constants
// =============================================

#define MAX_CPU_CORES 64  // Maximum number of CPU cores
#define MAX_PROC_NAME 256  // Maximum length for process names
#define MAX_ERROR_MSG 1024  // Maximum length for error messages
#define UI_REFRESH_RATE 1.0  // UI refresh rate in seconds
#define MAX_PROCESSES 1024   // Maximum number of processes

// Application version
#define SYSMON_VERSION_MAJOR     0
#define SYSMON_VERSION_MINOR     1
#define SYSMON_VERSION_PATCH     0
#define SYSMON_VERSION        "0.1.0"

// =============================================
// Data Structures
// =============================================

/**
 * @brief CPU usage metrics structure
 */
typedef struct {
    int num_cores;                      // Number of CPU cores detected
    double total_usage;                 // Total CPU usage percentage
    double core_usage[MAX_CPU_CORES];   // Per-core usage percentages
} cpu_metrics_t;

/**
 * @brief Memory usage metrics structure (all values in KB)
 */
typedef struct {
    // Physical memory
    unsigned long total;                // Total installed memory
    unsigned long free;                 // Free memory
    unsigned long available;            // Available memory (free + reclaimable)
    unsigned long used;                 // Used memory (calculated)
    unsigned long buffers;              // Memory used by buffers
    unsigned long cached;               // Memory used by cache
    unsigned long shared;               // Shared memory
    double usage_percent;               // Memory usage percentage

    // Swap memory
    unsigned long swap_total;           // Total swap space
    unsigned long swap_free;            // Free swap space
    unsigned long swap_used;            // Used swap space
    double swap_usage_percent;          // Swap usage percentage
} memory_metrics_t;

/**
 * @brief Network activity metrics structure
 */
typedef struct {
    char interface[16];                 // Interface name
    double rx_rate;                     // Receive rate (KB/s)
    double tx_rate;                     // Transmit rate (KB/s)
    double rx_utilization;              // Download utilization percentage
    double tx_utilization;              // Upload utilization percentage
    unsigned long total_rx;             // Total bytes received
    unsigned long total_tx;             // Total bytes transmitted
    unsigned long rx_bytes;             // Bytes received since last check
    unsigned long tx_bytes;             // Bytes transmitted since last check
} network_metrics_t;

/**
 * @brief Disk I/O metrics structure
 */
typedef struct {
    double read_rate;                   // Read rate (KB/s)
    double write_rate;                  // Write rate (KB/s)
    unsigned long total_read;           // Total bytes read
    unsigned long total_written;        // Total bytes written
} disk_metrics_t;

/**
 * @brief Process information structure
 */
typedef struct {
    pid_t pid;                          // Process ID
    char name[MAX_PROC_NAME];           // Process name
    double cpu_usage;                   // CPU usage percentage
    double mem_usage;                   // Memory usage percentage
    unsigned long mem_used;             // Memory used (KB)
    unsigned long long last_utime;      // Previous user time
    unsigned long long last_stime;      // Previous system time
} process_info_t;

/**
 * @brief Process metrics structure
 */
typedef struct {
    process_info_t processes[MAX_PROCESSES];  // Array of process info
    int count;                               // Number of processes
} process_metrics_t;

// Log levels for util functions
typedef enum {
    LOG_DEBUG,
    LOG_INFO, 
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

#endif /* SYSMON_H */
