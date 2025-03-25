/**
 * sysmon - Interactive System Monitor
 * 
 * error_handler.h - Error handling interface
 */

 #ifndef ERROR_HANDLER_H
 #define ERROR_HANDLER_H
 #define MAX_ERROR_MSG 1024
 
 #include <stdbool.h>

// Log severity levels
typedef enum {
    LOG_LEVEL_DEBUG,   // Debugging information
    LOG_LEVEL_INFO,    // Normal operational messages
    LOG_LEVEL_WARNING, // Potentially problematic situations
    LOG_LEVEL_ERROR,   // Error conditions
    LOG_LEVEL_FATAL    // Critical errors that terminate the program
} log_level_t;

// Initialize error handling system
bool error_handler_init(const char *log_dir);

// Log a formatted message with specified level
void log_message(log_level_t level, const char *format, ...);

// Convenience macros for logging
#define log_debug(...)    log_message(LOG_LEVEL_DEBUG, __VA_ARGS__)
#define log_info(...)     log_message(LOG_LEVEL_INFO, __VA_ARGS__)
#define log_warning(...)  log_message(LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_error(...)    log_message(LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_fatal(...)    log_message(LOG_LEVEL_FATAL, __VA_ARGS__)

// Clean up error handling resources
void error_handler_cleanup(void);

#endif /* ERROR_HANDLER_H */