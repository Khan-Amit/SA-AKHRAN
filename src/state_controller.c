#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>

// Global lifecycle flag definition
_Atomic int system_is_running = 0;

// Standalone real-time metrics for frontend UI Dial components
_Atomic uint64_t ui_snoop_deflected_count = 0;
_Atomic uint64_t ui_virus_purged_count = 0;
_Atomic uint64_t ui_delivered_count = 0;

static pthread_t ingress_worker;
static pthread_t filtration_worker;
static pthread_t egress_worker;
static pthread_t distribution_worker;

extern void* voip_ingress_engine(void* arg);
extern void* virus_filtration_engine(void* arg);
extern void* main_dump_distribution_hub(void* arg);
extern void* voip_egress_engine(void* arg);
extern void load_root_manifest_configuration(void);

// --- INTERACTIVE BUTTON RUN TRIGGER ---
void ui_trigger_on_click_run(void) {
    if (atomic_load_explicit(&system_is_running, memory_order_acquire) == 1) {
        return;
    }

    load_root_manifest_configuration();
    atomic_store_explicit(&system_is_running, 1, memory_order_release);

    pthread_create(&ingress_worker, NULL, voip_ingress_engine, NULL);
    pthread_create(&filtration_worker, NULL, virus_filtration_engine, NULL);
    pthread_create(&distribution_worker, NULL, main_dump_distribution_hub, NULL);
    pthread_create(&egress_worker, NULL, voip_egress_engine, NULL);
    
    printf("[App Dashboard UI] Engine activated. System entering network polling loops.\n");
}

// --- INTERACTIVE BUTTON REPORT TRIGGER ---
void ui_trigger_on_click_report(void) {
    // Switch to clean plain black text on crisp white background
    printf("\033[30%;47m"); 
    printf("\n====================================================================\n");
    printf("                  SA-AKHRAN TELEMETRY METRICS REPORT                 \n");
    printf("====================================================================\n");
    printf(" Ingress Snoop Deflections : %lu packets\n", atomic_load(&ui_snoop_deflected_count));
    printf(" Filtration Virus Purges   : %lu threats neutralized\n", atomic_load(&ui_virus_purged_count));
    printf(" Egress Untracked Deliveries: %lu cycles completed\n", atomic_load(&ui_delivered_count));
    printf("--------------------------------------------------------------------\n");

    FILE* sorter_log = fopen("public_sorter.log", "r");
    if (!sorter_log) {
        printf(" Public Sorter Ledger: Awaiting data inputs...\n");
    } else {
        char line_buffer[512];
        printf("--- EXTRACTED PUBLIC SORTER LOGS ---\n");
        while (fgets(line_buffer, sizeof(line_buffer), sorter_log)) {
            printf(" %s", line_buffer);
        }
        fclose(sorter_log);
    }

    printf("====================================================================\n\n");
    printf("\033[0m"); // Safely revert console layout options back to stock settings
}

// --- INTERACTIVE BUTTON STOP TRIGGER ---
void ui_trigger_on_click_stop(void) {
    if (atomic_load_explicit(&system_is_running, memory_order_acquire) == 0) {
        return;
    }

    printf("[App Dashboard UI] Shutdown instruction received. Terminating threads...\n");
    atomic_store_explicit(&system_is_running, 0, memory_order_release);

    pthread_join(ingress_worker, NULL);
    pthread_join(filtration_worker, NULL);
    pthread_join(distribution_worker, NULL);
    pthread_join(egress_worker, NULL);
    
    printf("[App Dashboard UI] Complete pipeline halted cleanly. System idled.\n");
}
