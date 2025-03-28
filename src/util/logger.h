/**
 * sysmon - Interactive System Monitor
 * 
 * logger.h - Thread-safe logging interface
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "../include/sysmon.h"  // For log_level_t
 
// Initialize logger with output file
bool logger_init(const char *filename);

// Thread-safe logging function
void logger_log(log_level_t level, const char *format, ...);

// Clean up logger resources 
void logger_cleanup(void);

const char *log_level_to_str(log_level_t level);

#endif /* LOGGER_H */

