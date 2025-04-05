/**
 * sysmon - Interactive System Monitor
 * 
 * error_handler.c - Error handling implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "error_handler.h"
#include "logger.h"
  
// Initialize error handling system
bool error_handler_init(const char *log_dir) 
{
    if (!log_dir) {
        fprintf(stderr, "Log directory path cannot be NULL\n");
        return false;
    }

    // Check if the log directory exists
    struct stat st;
    if (stat(log_dir, &st) == -1) {
        // Create directory if it doesn't exist
        if (mkdir(log_dir, 0755) == -1) {
            perror("Failed to create log directory");
            return false;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "Log path exists but is not a directory\n");
        return false;
    }



    // Generate timestamped log filename
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/sysmon_%04d%02d%02d_%02d%02d%02d.log",
            log_dir,
            tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
            tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);

    if (!logger_init(filename)) {
        fprintf(stderr, "Failed to initialize logger with file: %s\n", filename);
        return false;
    }

    log_info("sysmon initialized");
    return true;
}
 
// Core logging function
void log_message(log_level_t level, const char *format, ...) 
{
    va_list args;
    va_start(args, format);
    logger_log(level, format, args);
    va_end(args);

    // Exit on fatal errors
    if (level == LOG_FATAL) {
        error_handler_cleanup();
        exit(EXIT_FAILURE);
    }
}
 
// Convenience functions
void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logger_log(LOG_ERROR, format, args);
    va_end(args);
}

void log_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logger_log(LOG_WARNING, format, args);
    va_end(args);
}
 
void log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logger_log(LOG_INFO, format, args);
    va_end(args);
}
 
// Clean up error handling resources
void error_handler_cleanup(void) 
{
    log_info("sysmon shutting down");
    logger_cleanup();
}