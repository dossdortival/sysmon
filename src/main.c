/* sysmon - Interactive System Monitor
 * main.c - Entry point and application controller
 */

 #define _POSIX_C_SOURCE 199309L
 #include <signal.h>
 #include <stdlib.h>
 #include <time.h>
 #include <unistd.h> 
 
 #include "include/sysmon.h"
 #include "collector/cpu_collector.h"
 #include "collector/memory_collector.h"
 #include "collector/network_collector.h"
 #include "collector/disk_collector.h"
 #include "collector/process_collector.h"
 #include "ui/ui_manager.h"
 #include "util/error_handler.h"
 #include "util/logger.h"
 
// Global flag for graceful shutdown
static volatile sig_atomic_t g_shutdown_requested = 0;

static void handle_signal(int signal_number)
{
    (void)signal_number; // Unused parameter
    g_shutdown_requested = 1;
}

static int initialize_signal_handlers(void)
{
    const int signals[] = {SIGINT, SIGTERM};
    const size_t num_signals = sizeof(signals) / sizeof(signals[0]);
    
    for (size_t i = 0; i < num_signals; i++) {
        if (signal(signals[i], handle_signal) == SIG_ERR) {
            log_error("Failed to set up signal handler %d", signals[i]);
            return 0;
        }
    }
    return 1;
}

// Core initialization of all subsystems
bool initialize_subsystems(void)
{
    if (!error_handler_init("sysmon_error.log")) { 
        log_error("Failed to initialize error handling system");
        return false;
    }

    const struct {
        bool (*init_func)(void);
        const char *name;
    } subsystems[] = {
        {(bool(*)(void))cpu_collector_init, "CPU collector"},
        {(bool(*)(void))memory_collector_init, "Memory collector"},
        {(bool(*)(void))network_collector_init, "Network collector"},
        {(bool(*)(void))disk_collector_init, "Disk collector"},
        {(bool(*)(void))process_collector_init, "Process collector"},
        {(bool(*)(void))ui_init, "UI manager"}
    };

    for (size_t i = 0; i < sizeof(subsystems)/sizeof(subsystems[0]); i++) {
        if (!subsystems[i].init_func()) {
            log_error("%s initialization failed", subsystems[i].name);
            return false;
        }
    }

    return true;
}

// Collect and display metrics
static void collect_and_display_metrics(void)
{
    cpu_metrics_t cpu_metrics;
    memory_metrics_t memory_metrics;
    network_metrics_t network_metrics;
    disk_metrics_t disk_metrics;
    process_metrics_t process_metrics;

    struct {
        bool (*collect)(void*);
        void (*update)(const void*);
        void *data;
        const char *name;
    } collectors[] = {
        {
            .collect = (bool(*)(void*))cpu_collector_collect,
            .update = (void(*)(const void*))ui_update_cpu,
            .data = &cpu_metrics,
            .name = "CPU"
        },
        {
            .collect = (bool(*)(void*))memory_collector_collect,
            .update = (void(*)(const void*))ui_update_memory,
            .data = &memory_metrics,
            .name = "Memory"
        },
        {
            .collect = (bool(*)(void*))network_collector_collect,
            .update = (void(*)(const void*))ui_update_network,
            .data = &network_metrics,
            .name = "Network"
        },
        {
            .collect = (bool(*)(void*))disk_collector_collect,
            .update = (void(*)(const void*))ui_update_disk,
            .data = &disk_metrics,
            .name = "Disk"
        },
        {
            .collect = (bool(*)(void*))process_collector_collect,
            .update = (void(*)(const void*))ui_update_processes,
            .data = &process_metrics,
            .name = "Process"
        }
    };

    for (size_t i = 0; i < sizeof(collectors)/sizeof(collectors[0]); i++) {
        if (!collectors[i].collect(collectors[i].data)) {
            log_error("%s data collection failed", collectors[i].name);
            continue;
        }
        collectors[i].update(collectors[i].data);
    }
}

// Main application loop
static void main_loop(void)
{
    struct timespec last_update;
    clock_gettime(CLOCK_MONOTONIC, &last_update);

    while (!g_shutdown_requested) {
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);

        double time_diff = (current_time.tv_sec - last_update.tv_sec) + 
                         (current_time.tv_nsec - last_update.tv_nsec) / 1e9;

        if (time_diff >= 1.0) {
            collect_and_display_metrics();
            ui_refresh();
            last_update = current_time;
        }

        ui_handle_input();
        sleep(0.1); // 10ms sleep to reduce CPU usage
    }
}

// cleanup all subsystems
static void cleanup_subsystems(void)
{
    ui_cleanup();
    process_collector_cleanup();
    disk_collector_cleanup();
    network_collector_cleanup();
    memory_collector_cleanup();
    cpu_collector_cleanup();
    error_handler_cleanup();
}

// main function
int main(int argc, char *argv[])
{
    (void)argc; (void)argv; // Unused parameters

    if (!initialize_signal_handlers()) {
        return EXIT_FAILURE;
    }

    if (!initialize_subsystems()) {
        return EXIT_FAILURE;
    }

    main_loop();
    cleanup_subsystems();

    log_info("sysmon terminated successfully");
    return EXIT_SUCCESS;
}