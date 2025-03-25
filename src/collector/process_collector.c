/**
 * sysmon - Interactive System Monitor
 * 
 * process_collector.c - Process statistics collector implementation
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>

#include "process_collector.h"
#include "../util/error_handler.h"
#include "../util/logger.h"
 
static unsigned long long prev_total_jiffies = 0;
static unsigned long long prev_work_jiffies = 0;
 
 /**
  * @brief Reads process statistics from /proc filesystem
  * @param pid Process ID to read
  * @param process Pointer to process structure to populate
  * @return true if successful, false otherwise
  */
static bool read_process_stat(pid_t pid, struct {
    int pid;
    char name[MAX_PROC_NAME];
    double cpu_usage;
    double mem_usage;
    unsigned long mem_used;
} *process) 
{
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

    // Clean process name (remove parentheses)
    size_t len = strlen(name);
    if (len > 1) {
        memmove(name, name+1, len-2);
        name[len-2] = '\0';
    }

    strncpy(process->name, name, MAX_PROC_NAME);
    process->pid = pid;
    process->cpu_usage = utime + stime;
    process->mem_used = rss * (sysconf(_SC_PAGE_SIZE) / 1024);
    process->mem_usage = 0;

    return true;
}
 
 /**
  * @brief Calculates CPU usage percentages for all processes
  * @param metrics Process metrics structure
  * @param total_jiffies_diff Time difference in jiffies
  */
static void calculate_cpu_usage(process_metrics_t *metrics, 
                                unsigned long long total_jiffies_diff)
{
    if (total_jiffies_diff == 0) return;

    for (int i = 0; i < metrics->count; i++) {
        metrics->processes[i].cpu_usage = 
            (metrics->processes[i].cpu_usage * 100.0) / total_jiffies_diff;
    }
}
 
 /**
  * @brief Comparison function for sorting processes by CPU usage
  * @param a First process to compare
  * @param b Second process to compare
  * @return Comparison result
  */
 static int compare_processes(const void *a, const void *b)
{
    const struct {
        int pid;
        char name[MAX_PROC_NAME];
        double cpu_usage;
        double mem_usage;
        unsigned long mem_used;
    } *pa = a, *pb = b;
    
    if (pa->cpu_usage > pb->cpu_usage) return -1;
    if (pa->cpu_usage < pb->cpu_usage) return 1;
    return 0;
}
 
bool process_collector_collect(process_metrics_t *metrics)
{
    if (!metrics) return false;

    DIR *dir;
    struct dirent *entry;
    metrics->count = 0;
    unsigned long long total_jiffies = 0;
    unsigned long long work_jiffies = 0;

    // Read total CPU jiffies from /proc/stat
    FILE *stat_fp = fopen("/proc/stat", "r");
    if (!stat_fp) {
        log_error("Failed to open /proc/stat");
        return false;
    }

    char line[256];
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

    // Scan /proc directory for processes
    dir = opendir("/proc");
    if (!dir) {
        log_error("Failed to open /proc");
        return false;
    }

    while ((entry = readdir(dir)) != NULL && metrics->count < MAX_PROCESSES) {
        if (!isdigit(entry->d_name[0])) continue;

        pid_t pid = atoi(entry->d_name);
        if (pid <= 1) continue;

        if (read_process_stat(pid, &metrics->processes[metrics->count])) {
            metrics->count++;
        }
    }
    closedir(dir);

    // Calculate CPU and memory usage
    unsigned long long total_diff = total_jiffies - prev_total_jiffies;
    if (prev_total_jiffies > 0 && total_diff > 0) {
        calculate_cpu_usage(metrics, total_diff);
    }

    qsort(metrics->processes, metrics->count, sizeof(metrics->processes[0]), compare_processes);

    long total_memory = sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE) / 1024;
    if (total_memory > 0) {
        for (int i = 0; i < metrics->count; i++) {
            metrics->processes[i].mem_usage = 
                (metrics->processes[i].mem_used * 100.0) / total_memory;
        }
    }

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