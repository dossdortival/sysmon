#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>

#define MAX_PROCESSES 256  // Define max processes to track

// Structure to store process information
typedef struct {
    int pid;         // Process ID
    char name[256];  // Process name
    double cpu_usage; // CPU usage percentage
    long mem_usage;  // Memory usage in KB
} ProcessInfo;

// Function to get the number of running processes
int get_running_processes();

// Function to retrieve active processes and their resource usage
size_t get_active_processes(ProcessInfo processes[], size_t max_processes);

#endif // PROCESS_H
