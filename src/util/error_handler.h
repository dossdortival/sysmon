/**
 * sysmon - Interactive System Monitor
 * 
 * error_handler.h - Error handling and logging utilities
 */

 #ifndef ERROR_HANDLER_H
 #define ERROR_HANDLER_H
 #define MAX_ERROR_MSG 1024
 
 #include <stdbool.h>
 
 
 // Log levels
 typedef enum {
     LOG_DEBUG,
     LOG_INFO,
     LOG_WARNING,
     LOG_ERROR,
     LOG_FATAL
 } log_level;
 
 // Initialize error handling system
 bool error_handler_init(void);
 
 // Log a message with specific log level
 void log_message(log_level level, const char *format, ...);
 
 // Convenience functions for different log levels
 void log_debug(const char *format, ...);
 void log_info(const char *format, ...);
 void log_warning(const char *format, ...);
 void log_error(const char *format, ...);
 void log_fatal(const char *format, ...);
 
 // Clean up error handling resources
 void error_handler_cleanup(void);
 
 #endif /* ERROR_HANDLER_H */