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
 #include <pthread.h>
 
 #include "logger.h"
 
 static FILE *log_stream = NULL;
 static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
 
 bool logger_init(const char *filename) 
 {
     pthread_mutex_lock(&log_mutex);
     
     if (log_stream) {
         pthread_mutex_unlock(&log_mutex);
         return false;
     }
 
     log_stream = fopen(filename, "a");
     if (!log_stream) {
         pthread_mutex_unlock(&log_mutex);
         return false;
     }
 
     pthread_mutex_unlock(&log_mutex);
     return true;
 }
 
 void logger_log(log_level_t level, const char *format, ...) 
 {
     pthread_mutex_lock(&log_mutex);
     
     if (!log_stream) {
         pthread_mutex_unlock(&log_mutex);
         return;
     }
 
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
     fprintf(log_stream, "[%s] [%5s] %s\n", 
             timestamp, log_level_to_str(level), message);
     fflush(log_stream);
 
     pthread_mutex_unlock(&log_mutex);
 
     // Pass to error handler for stderr output
     if (level >= LOG_LEVEL_ERROR) {
         log_message(level, "%s", message);
     }
 }
 
 void logger_flush(void) 
 {
     pthread_mutex_lock(&log_mutex);
     if (log_stream) {
         fflush(log_stream);
     }
     pthread_mutex_unlock(&log_mutex);
 }
 
 void logger_cleanup(void) 
 {
     pthread_mutex_lock(&log_mutex);
     if (log_stream) {
         fclose(log_stream);
         log_stream = NULL;
     }
     pthread_mutex_unlock(&log_mutex);
 }