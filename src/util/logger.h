/**
 * sysmon - Interactive System Monitor
 * 
 * logger.h - Thread-safe logging interface
 */

 #ifndef LOGGER_H
 #define LOGGER_H
 
 #include "error_handler.h"
 
 // Initialize logger with output file
 bool logger_init(const char *filename);
 
 // Thread-safe logging function
 void logger_log(log_level_t level, const char *format, ...);
 
 // Flush any buffered log messages
 void logger_flush(void);
 
 // Clean up logger resources
 void logger_cleanup(void);
 
 #endif /* LOGGER_H */