#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

// Function to get the number of running processes
int get_running_processes() {
    FILE *file = fopen("/proc/stat", "r");
    if (!file) {
        perror("Failed to open /proc/stat");
        return -1;
    }

    char line[256];
    int running_processes = 0;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "procs_running %d", &running_processes) == 1) {
            break;
        }
    }

    fclose(file);
    return running_processes;
}

// Function to retrieve active processes and their CPU & MEM usage
size_t get_active_processes(ProcessInfo processes[], size_t max_processes) {
    DIR *dir = opendir("/proc");
    if (!dir) {
        perror("Failed to open /proc");
        return 0;
    }

    struct dirent *entry;
    size_t count = 0;

    while ((entry = readdir(dir)) && count < max_processes) {
        struct stat entry_stat;
        char entry_path[256];

        //snprintf(entry_path, sizeof(entry_path), "/proc/%.250s", entry->d_name);
        //entry_path[sizeof(entry_path) - 1] = '\0'; // Ensure null termination

        snprintf(entry_path, sizeof(entry_path), "/proc/%.*s", 
         (int)(sizeof(entry_path) - 7), entry->d_name);

        // Use stat to check if it's a directory
        if (stat(entry_path, &entry_stat) == 0 && S_ISDIR(entry_stat.st_mode)) {
            int pid = atoi(entry->d_name);
            if (pid > 0) {
                char stat_path[256], status_path[256];
                FILE *stat_file, *status_file;
                long utime, stime, memory;
                char name[256];

                // Read process name and CPU usage from /proc/[pid]/stat
                snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
                stat_file = fopen(stat_path, "r");
                if (stat_file) {
                    fscanf(stat_file, "%*d (%255[^)]) %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld",
                           name, &utime, &stime);
                    fclose(stat_file);

                    double cpu_usage = (utime + stime) / (double)sysconf(_SC_CLK_TCK);
                    
                    // Read memory usage from /proc/[pid]/status
                    snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
                    status_file = fopen(status_path, "r");
                    memory = 0;
                    if (status_file) {
                        char line[256];
                        while (fgets(line, sizeof(line), status_file)) {
                            if (sscanf(line, "VmRSS: %ld kB", &memory) == 1) {
                                break;
                            }
                        }
                        fclose(status_file);
                    }

                    // Store process info
                    processes[count].pid = pid;
                    strncpy(processes[count].name, name, sizeof(processes[count].name) - 1);
                    processes[count].cpu_usage = cpu_usage;
                    processes[count].mem_usage = memory;
                    count++;
                }
            }
        }
    }

    closedir(dir);
    return count;
}

