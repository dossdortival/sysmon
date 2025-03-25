/**
 * sysmon - Interactive System Monitor
 * 
 * memory_collector.c - Memory statistics collector implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory_collector.h"
#include "../util/error_handler.h"

// File containing memory statistics
#define PROC_MEMINFO_PATH "/proc/meminfo"

// Memory field identifiers
typedef enum {
    MEM_TOTAL,
    MEM_FREE,
    MEM_AVAILABLE,
    BUFFERS,
    CACHED,
    SHMEM,
    SWAP_TOTAL,
    SWAP_FREE,
    NUM_FIELDS
} mem_field;

// Field names and their corresponding positions in /proc/meminfo
static const struct {
    const char *name;
    size_t name_len;
} mem_fields[NUM_FIELDS] = {
    [MEM_TOTAL]    = {"MemTotal:", 9},
    [MEM_FREE]     = {"MemFree:", 8},
    [MEM_AVAILABLE] = {"MemAvailable:", 13},
    [BUFFERS]      = {"Buffers:", 8},
    [CACHED]       = {"Cached:", 7},
    [SHMEM]        = {"Shmem:", 6},
    [SWAP_TOTAL]   = {"SwapTotal:", 10},
    [SWAP_FREE]    = {"SwapFree:", 9}
};

bool memory_collector_init(void) {
    // Verify we can read the memory info file
    FILE *file = fopen(PROC_MEMINFO_PATH, "r");
    if (file == NULL) {
        log_error("Failed to open %s", PROC_MEMINFO_PATH);
        return false;
    }
    fclose(file);
    return true;
}

bool memory_collector_collect(memory_data *data) {
    if (data == NULL) {
        log_error("Invalid data pointer");
        return false;
    }

    FILE *file = fopen(PROC_MEMINFO_PATH, "r");
    if (file == NULL) {
        log_error("Failed to open %s", PROC_MEMINFO_PATH);
        return false;
    }

    // Initialize all values to 0
    unsigned long values[NUM_FIELDS] = {0};
    char line[256];

    // Read and parse each line of /proc/meminfo
    while (fgets(line, sizeof(line), file)) {
        for (int i = 0; i < NUM_FIELDS; i++) {
            if (strncmp(line, mem_fields[i].name, mem_fields[i].name_len) == 0) {
                sscanf(line + mem_fields[i].name_len, "%lu", &values[i]);
                break;
            }
        }
    }
    fclose(file);

    // Calculate derived values
    data->total = values[MEM_TOTAL];
    data->free = values[MEM_FREE];
    data->available = values[MEM_AVAILABLE];
    data->buffers = values[BUFFERS];
    data->cached = values[CACHED];
    data->shared = values[SHMEM];
    
    // Used memory calculation (adjust for shared memory)
    data->used = data->total - data->free - data->buffers - data->cached + data->shared;
    
    // Calculate usage percentages
    data->usage_percent = (data->total > 0) ? 100.0 * data->used / data->total : 0.0;
    
    // Swap information
    data->swap_total = values[SWAP_TOTAL];
    data->swap_free = values[SWAP_FREE];
    data->swap_used = data->swap_total - data->swap_free;
    data->swap_usage_percent = (data->swap_total > 0) ? 
        100.0 * data->swap_used / data->swap_total : 0.0;

    return true;
}

void memory_collector_cleanup(void) {
    // No resources to clean up
}

