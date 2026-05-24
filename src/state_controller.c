#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>

// Global lifecycle controls
_Atomic int system_is_running = 0;

// Thread tracking handles
static pthread_t ingress_worker;
static pthread_t filtration_worker;
static pthread_t egress_worker;
static pthread_t distribution_worker;

// Link external worker loops from your isolated engine modules
extern void* voip_ingress_engine(void* arg);
extern void* virus_filtration_engine(void* arg);
extern void* main_dump_distribution_hub(void* arg);
extern void* voip_egress_engine(void* arg);

// --- INTERACTIVE TRIGGER 1: THE RUN COMMAND ---
void ui_trigger_on_click_run(void) {
    // Prevent starting an already active engine instance
    if (atomic_load_explicit(&system_is_running, memory_order_acquire) == 1) {
        return;
    }

    // Set engine to active state
    atomic_store_explicit(&system_is_running, 1, memory_order_release);

    // Boot up the unlinked processing systems across separate threads
    pthread_create(&ingress_worker, NULL, voip_ingress_engine, NULL);
    pthread_create(&filtration_worker, NULL, virus_filtration_engine, NULL);
    pthread_create(&distribution_worker, NULL, main_dump_distribution_hub, NULL);
    pthread_create(&egress_worker, NULL, voip_egress_engine, NULL);
}

// --- INTERACTIVE TRIGGER 2: THE REPORT COMMAND ---
void ui_trigger_on_click_report(void) {
    // Plain black text on white background terminal reader layout
    printf("\033[30%;47m"); // ANSI Escape: Set text color to Black, background to White
    printf("\n==================================================\n");
    printf("              AKHRAN ENGINE REPORT LAYOUT          \n");
    printf("==================================================\n\n");

    FILE* sorter_log = fopen("public_sorter.log", "r");
    if (!sorter_log) {
        printf("No public sorting data recorded yet.\n");
    } else {
        char file_line[256];
        printf("--- PUBLIC SORTER ENTRIES ---\n");
        while (fgets(file_line, sizeof(file_line), sorter_log)) {
            printf("%s", file_line);
        }
        fclose(sorter_log);
    }

    printf("\n==================================================\n");
    printf("\033[0m"); // Reset colors back to standard console defaults
}

// --- INTERACTIVE TRIGGER 3: THE STOP COMMAND ---
void ui_trigger_on_click_stop(void) {
    // If system is already stopped, bypass execution
    if (atomic_load_explicit(&system_is_running, memory_order_acquire) == 0) {
        return;
    }

    // Signal all internal worker threads to drop out of execution loops
    atomic_store_explicit(&system_is_running, 0, memory_order_release);

    // Wait for components to safely release shared memory pools
    pthread_join(ingress_worker, NULL);
    pthread_join(filtration_worker, NULL);
    pthread_join(distribution_worker, NULL);
    pthread_join(egress_worker, NULL);
}
