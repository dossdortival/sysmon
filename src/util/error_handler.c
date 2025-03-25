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

// Log file handle
static FILE *log_file = NULL;

// Convert log level to string
static const char *log_level_to_str(log_level_t level) 
{
    static const char *level_strings[] = {
        [LOG_LEVEL_DEBUG]   = "DEBUG",
        [LOG_LEVEL_INFO]    = "INFO",
        [LOG_LEVEL_WARNING] = "WARN",
        [LOG_LEVEL_ERROR]   = "ERROR",
        [LOG_LEVEL_FATAL]   = "FATAL"
    };
    return (level < sizeof(level_strings)/sizeof(level_strings[0])) ? 
           level_strings[level] : "UNKNOWN";
}

// Create directory if it doesn't exist
static bool ensure_dir_exists(const char *path) 
{
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        return mkdir(path, 0755) == 0;
    }
    return S_ISDIR(st.st_mode);
}

// Initialize error handling system
bool error_handler_init(const char *log_dir) 
{
    if (!log_dir) {
        fprintf(stderr, "Log directory path cannot be NULL\n");
        return false;
    }

    if (!ensure_dir_exists(log_dir)) {
        fprintf(stderr, "Failed to create log directory: %s\n", log_dir);
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

    log_file = fopen(filename, "a");
    if (!log_file) {
        fprintf(stderr, "Failed to open log file: %s\n", filename);
        return false;
    }

    log_info("sysmon initialized");
    return true;
}

// Core logging function
void log_message(log_level_t level, const char *format, ...) 
{
    if (!log_file) return;

    // Get timestamp
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &tm_now);

    // Format the message
    char message[MAX_ERROR_MSG];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Write to log file
    fprintf(log_file, "[%s] [%5s] %s\n", 
            timestamp, log_level_to_str(level), message);
    fflush(log_file);

    // Write to stderr for errors
    if (level >= LOG_LEVEL_ERROR) {
        fprintf(stderr, "[%s] [%5s] %s\n", 
                timestamp, log_level_to_str(level), message);
    }

    // Exit on fatal errors
    if (level == LOG_LEVEL_FATAL) {
        error_handler_cleanup();
        exit(EXIT_FAILURE);
    }
}

// Clean up error handling resources
void error_handler_cleanup(void) 
{
    if (log_file) {
        log_info("sysmon shutting down");
        fclose(log_file);
        log_file = NULL;
    }
}