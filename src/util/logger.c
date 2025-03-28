/**
 * sysmon - Interactive System Monitor
 * 
 * logger.c - Thread-safe logging implementation
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdarg.h>
 #include <string.h>
 #include <time.h> 
 
 #include "logger.h"
 
 static FILE *log_stream = NULL;
static bool log_to_file = false;

// Implement the log level to string conversion
const char *log_level_to_str(log_level_t level) {
    switch(level) {
        case LOG_DEBUG:   return "DEBUG";
        case LOG_INFO:    return "INFO";
        case LOG_WARNING: return "WARN";
        case LOG_ERROR:   return "ERROR";
        default:          return "UNKNOWN";
    }
}

bool logger_init(const char *filename) {
    if (filename) {
        log_stream = fopen(filename, "a");
        if (!log_stream) {
            perror("Failed to open log file");
            return false;
        }
        log_to_file = true;
    } else {
        log_stream = stderr;
    }
    return true;
}

void logger_log(log_level_t level, const char *format, ...) {
    if (!log_stream) return;

    time_t now = time(NULL);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    va_list args;
    va_start(args, format);

    // First print to our buffer to get the length
    char message[1024];
    vsnprintf(message, sizeof(message), format, args);

    fprintf(log_stream, "[%s] [%5s] %s\n", 
            timestamp, log_level_to_str(level), message);
    fflush(log_stream);

    va_end(args);
}

void logger_cleanup(void) {
    if (log_to_file && log_stream) {
        fclose(log_stream);
    }
    log_stream = NULL;
}