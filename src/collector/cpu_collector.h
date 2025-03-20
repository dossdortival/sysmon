/**
 * sysmon - Interactive System Monitor
 * 
 * cpu_collector.h - CPU statistics collector
 */

 #ifndef CPU_COLLECTOR_H
 #define CPU_COLLECTOR_H
 
 #include "../include/sysmon.h"
 
 // Initialize the CPU collector
 bool cpu_collector_init(void);
 
 // Collect CPU data
 bool cpu_collector_collect(cpu_data *data);
 
 // Clean up CPU collector resources
 void cpu_collector_cleanup(void);
 
 #endif /* CPU_COLLECTOR_H */