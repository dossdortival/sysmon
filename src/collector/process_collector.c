/**
 * sysmon - Interactive System Monitor
 * 
 * process_collector.c - Process statistics collector implementation
 */

#define _POSIX_C_SOURCE 200809L
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/stat.h> 
#include <time.h> 

#include "process_collector.h"
#include "../util/error_handler.h"
#include "../util/logger.h"
 
static unsigned long long prev_total_jiffies = 0; // Previous total CPU jiffies (time units)
static unsigned long long prev_work_jiffies = 0;  // Previous work CPU jiffies (time units)
static process_info_t prev_processes[MAX_PROCESSES];
static int prev_process_count = 0;

static bool is_kernel_thread(pid_t pid) {
    char stat_path[64];
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/cmdline", pid);
    
    FILE *fp = fopen(stat_path, "r");
    if (!fp) return true;
    
    int ch = fgetc(fp);
    fclose(fp);
    return ch == EOF;
}

// Process-specific statistics from /proc/[pid]/stat
static bool read_process_stat(pid_t pid, process_info_t *process) {
    char stat_path[64];
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);

    FILE *fp = fopen(stat_path, "r");
    if (!fp) return false;

    char name[MAX_PROC_NAME];
    char state;
    unsigned long utime, stime;
    long rss;
    unsigned long long starttime;

    if (fscanf(fp, "%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u "
                  "%lu %lu %*d %*d %*d %*d %*u %llu",
             name, &state, &utime, &stime, &starttime) != 5) {
        fclose(fp);
        return false;
    }

    if (fscanf(fp, "%ld", &rss) != 1) {
        fclose(fp);
        return false;
    }
    fclose(fp);

    // Remove parentheses from process name
    size_t len = strlen(name);
    if (len > 1) {
        memmove(name, name+1, len-2);
        name[len-2] = '\0';
    }

    strncpy(process->name, name, MAX_PROC_NAME);
    process->pid = pid;
    process->last_utime = utime;
    process->last_stime = stime;
    process->mem_used = rss * (sysconf(_SC_PAGE_SIZE) / 1024);
    process->cpu_usage = 0.0;
    process->mem_usage = 0.0;

    return true;
}

static int compare_processes(const void *a, const void *b) {
    const process_info_t *pa = a;
    const process_info_t *pb = b;

    // First sort by CPU% descending
    if (pb->cpu_usage > pa->cpu_usage) return 1;
    if (pb->cpu_usage < pa->cpu_usage) return -1;
    
    // Then by MEM% descending
    if (pb->mem_usage > pa->mem_usage) return 1;
    if (pb->mem_usage < pa->mem_usage) return -1;
    
    // Finally by PID ascending
    return (pa->pid > pb->pid) ? 1 : -1;
}

bool process_collector_collect(process_metrics_t *metrics) {
    // Get total CPU jiffies
    FILE *stat_fp = fopen("/proc/stat", "r");
    if (!stat_fp) return false;

    char line[256];
    unsigned long long total_jiffies = 0;
    unsigned long long work_jiffies = 0;

    while (fgets(line, sizeof(line), stat_fp)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            unsigned long user, nice, system, idle, iowait, irq, softirq;
            sscanf(line + 5, "%lu %lu %lu %lu %lu %lu %lu",
                  &user, &nice, &system, &idle, &iowait, &irq, &softirq);
            work_jiffies = user + nice + system + irq + softirq;
            total_jiffies = work_jiffies + idle + iowait;
            break;
        }
    }
    fclose(stat_fp);

    // Scan /proc for processes
    DIR *dir = opendir("/proc");
    if (!dir) return false;

    metrics->count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL && metrics->count < MAX_PROCESSES) {
        if (!isdigit(entry->d_name[0])) continue;

        pid_t pid = atoi(entry->d_name);
        if (pid <= 1 || is_kernel_thread(pid)) continue;

        if (read_process_stat(pid, &metrics->processes[metrics->count])) {
            metrics->count++;
        }
    }
    closedir(dir);

    // Calculate CPU usage if we have previous data
    if (prev_total_jiffies > 0 && total_jiffies > prev_total_jiffies) {
        unsigned long long total_diff = total_jiffies - prev_total_jiffies;

        for (int i = 0; i < metrics->count; i++) {
            for (int j = 0; j < prev_process_count; j++) {
                if (metrics->processes[i].pid == prev_processes[j].pid) {
                    unsigned long long utime_diff = metrics->processes[i].last_utime - prev_processes[j].last_utime;
                    unsigned long long stime_diff = metrics->processes[i].last_stime - prev_processes[j].last_stime;
                    unsigned long long process_diff = utime_diff + stime_diff;
                    
                    metrics->processes[i].cpu_usage = (process_diff * 100.0) / total_diff;
                    break;
                }
            }
        }
    }

    // Calculate memory usage
    long total_memory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE) / 1024;
    if (total_memory > 0) {
        for (int i = 0; i < metrics->count; i++) {
            metrics->processes[i].mem_usage = (metrics->processes[i].mem_used * 100.0) / total_memory;
        }
    }

    // Sort by CPU usage
    qsort(metrics->processes, metrics->count, sizeof(process_info_t), compare_processes);

    // Save current state for next iteration
    memcpy(prev_processes, metrics->processes, metrics->count * sizeof(process_info_t));
    prev_process_count = metrics->count;
    prev_total_jiffies = total_jiffies;
    prev_work_jiffies = work_jiffies;

    return true;
}

 
bool process_collector_init(void)
{
    prev_total_jiffies = 0;
    prev_work_jiffies = 0;
    return true;
}

void process_collector_cleanup(void)
{
    // No resources to clean up
}