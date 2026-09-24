#ifndef process_h
#define process_h

#include "common.h"
#include "kernel.h"
#include "virtual.h"

#define PROCS_MAX 8       // Maximum number of processes

#define PROC_UNUSED   0   // Unused process control structure
#define PROC_RUNNABLE 1   // Runnable process
#define PROC_EXITED 2


extern struct process *current_proc; //currently running process

struct process *create_process(const void *image, size_t image_size);
void process_init(void);
void yield(void);
void switch_context(uint64_t *prev_sp, uint64_t *next_sp);
void switch_page_table(uint64_t satp_value, uint64_t sscratch_value);
void delay(int n);
int process_id(void);
void exit_process(void);

#endif