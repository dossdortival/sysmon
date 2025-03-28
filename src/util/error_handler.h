/**
 * sysmon - Interactive System Monitor
 * 
 * error_handler.h - Error handling interface
 */

 #ifndef ERROR_HANDLER_H
 #define ERROR_HANDLER_H
 #define MAX_ERROR_MSG 1024
 
 #include "../include/sysmon.h"  // For log_level_t
 
bool error_handler_init(const char *log_dir);
void log_error(const char *format, ...);
void log_warning(const char *format, ...);
void log_info(const char *format, ...);
void log_message(log_level_t level, const char *format, ...); 
void error_handler_cleanup(void);

#endif /* ERROR_HANDLER_H */