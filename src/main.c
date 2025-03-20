/**
 * sysmon - Interactive System Monitor
 * 
 * main.c - Entry point and application controller
 */

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <signal.h>
 #include <ncurses.h>
 #include <time.h>
 #include <string.h>
 
 #include "include/sysmon.h"
 #include "collector/cpu_collector.h"
 #include "collector/memory_collector.h"
 #include "ui/ui_manager.h"
 #include "util/error_handler.h"
 
 // Global flag for program termination
 volatile sig_atomic_t running = 1;
 
 // Signal handler for graceful termination
 void handle_signal(int sig) {
    (void)sig; // Unused
     running = 0;
 }
 
 int main(int argc, char *argv[]) {
    (void)argc; // Unused
    (void)argv; // Unused

     // Initialize signal handling
     signal(SIGINT, handle_signal);
     signal(SIGTERM, handle_signal);
     
     // Initialize error handling
     if (!error_handler_init()) {
         fprintf(stderr, "Failed to initialize error handling system\n");
         return EXIT_FAILURE;
     }
     
     // Initialize data collectors
     if (!cpu_collector_init()) {
         log_error("Failed to initialize CPU collector");
         return EXIT_FAILURE;
     }
     
     if (!memory_collector_init()) {
         log_error("Failed to initialize memory collector");
         cpu_collector_cleanup();
         return EXIT_FAILURE;
     }
     
     // Initialize UI
     if (!ui_init()) {
         log_error("Failed to initialize UI");
         memory_collector_cleanup();
         cpu_collector_cleanup();
         return EXIT_FAILURE;
     }
     
     // Main application loop
     struct timespec last_update = {0}, current_time = {0};
     clock_gettime(CLOCK_MONOTONIC, &last_update);
     
     while (running) {
         // Get current time
         clock_gettime(CLOCK_MONOTONIC, &current_time);
         
         // Calculate time difference
         double time_diff = (current_time.tv_sec - last_update.tv_sec) + 
                           (current_time.tv_nsec - last_update.tv_nsec) / 1e9;
         
         // Update every second
         if (time_diff >= 1.0) {
             // Collect data
             cpu_data cpu_info;
             memory_data mem_info;
             
             if (!cpu_collector_collect(&cpu_info)) {
                 log_error("Failed to collect CPU data");
             }
             
             if (!memory_collector_collect(&mem_info)) {
                 log_error("Failed to collect memory data");
             }
             
             // Update UI with collected data
             ui_update_cpu(&cpu_info);
             ui_update_memory(&mem_info);
             ui_refresh();
             
             // Update last update time
             last_update = current_time;
         }
         
         // Check for user input
         ui_handle_input();
         
         // Sleep to avoid hogging CPU
         sleep(1); // 1 second
     }
     
     // Cleanup
     ui_cleanup();
     memory_collector_cleanup();
     cpu_collector_cleanup();
     error_handler_cleanup();
     
     printf("sysmon terminated\n");
     return EXIT_SUCCESS;
 }