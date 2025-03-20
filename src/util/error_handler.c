/**
 * sysmon - Interactive System Monitor
 * 
 * error_handler.c - Error handling and logging implementation
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdarg.h>
 #include <time.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 
 #include "error_handler.h"
 
 // Log file
 static FILE *log_file = NULL;
 
 // Convert log level to string
 static const char *log_level_str(log_level level) {
     switch (level) {
         case LOG_DEBUG:   return "DEBUG";
         case LOG_INFO:    return "INFO";
         case LOG_WARNING: return "WARNING";
         case LOG_ERROR:   return "ERROR";
         case LOG_FATAL:   return "FATAL";
         default:          return "UNKNOWN";
     }
 }
 
 bool error_handler_init(void) {
     // Create logs directory if it doesn't exist
     struct stat st = {0};
     if (stat("logs", &st) == -1) {
         if (mkdir("logs", 0755) != 0) {
             fprintf(stderr, "Failed to create logs directory\n");
             return false;
         }
     }
     
     // Get current time for log filename
     time_t now = time(NULL);
     struct tm *tm_now = localtime(&now);
     
     char filename[64];
     strftime(filename, sizeof(filename), "logs/sysmon_%Y%m%d_%H%M%S.log", tm_now);
     
     // Open log file
     log_file = fopen(filename, "w");
     if (log_file == NULL) {
         fprintf(stderr, "Failed to open log file: %s\n", filename);
         return false;
     }
     
     // Initial log message
     log_info("sysmon started");
     
     return true;
 }
 
 void log_message(log_level level, const char *format, ...) {
     if (log_file == NULL) {
         return;
     }
     
     // Get current time
     time_t now = time(NULL);
     struct tm *tm_now = localtime(&now);
     
     char timestamp[20];
     strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_now);
     
     // Write log prefix
     fprintf(log_file, "[%s] [%s] ", timestamp, log_level_str(level));
     
     // Write log message
     va_list args;
     va_start(args, format);
     vfprintf(log_file, format, args);
     va_end(args);
     
     // Add newline
     fprintf(log_file, "\n");
     
     // Flush to ensure message is written immediately
     fflush(log_file);
     
     // Also print to stderr for ERROR and FATAL
     if (level >= LOG_ERROR) {
         fprintf(stderr, "[%s] %s: ", timestamp, log_level_str(level));
         va_start(args, format);
         vfprintf(stderr, format, args);
         va_end(args);
         fprintf(stderr, "\n");
     }
     
     // Exit on FATAL errors
     if (level == LOG_FATAL) {
         exit(EXIT_FAILURE);
     }
 }
 
 void log_debug(const char *format, ...) {
     va_list args;
     va_start(args, format);
     char message[MAX_ERROR_MSG];
     vsnprintf(message, sizeof(message), format, args);
     va_end(args);
     
     log_message(LOG_DEBUG, "%s", message);
 }
 
 void log_info(const char *format, ...) {
     va_list args;
     va_start(args, format);
     char message[MAX_ERROR_MSG];
     vsnprintf(message, sizeof(message), format, args);
     va_end(args);
     
     log_message(LOG_INFO, "%s", message);
 }
 
 void log_warning(const char *format, ...) {
     va_list args;
     va_start(args, format);
     char message[MAX_ERROR_MSG];
     vsnprintf(message, sizeof(message), format, args);
     va_end(args);
     
     log_message(LOG_WARNING, "%s", message);
 }
 
 void log_error(const char *format, ...) {
     va_list args;
     va_start(args, format);
     char message[MAX_ERROR_MSG];
     vsnprintf(message, sizeof(message), format, args);
     va_end(args);
     
     log_message(LOG_ERROR, "%s", message);
 }
 
 void log_fatal(const char *format, ...) {
     va_list args;
     va_start(args, format);
     char message[MAX_ERROR_MSG];
     vsnprintf(message, sizeof(message), format, args);
     va_end(args);
     
     log_message(LOG_FATAL, "%s", message);
     // This will exit the program
 }
 
 void error_handler_cleanup(void) {
     if (log_file != NULL) {
         log_info("sysmon terminated");
         fclose(log_file);
         log_file = NULL;
     }
 }