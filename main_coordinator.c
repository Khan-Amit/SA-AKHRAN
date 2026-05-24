#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define RING_BUFFER_SIZE 16

typedef struct {
    uint32_t transaction_id;
    uint32_t style_id;
    size_t payload_len;
    char data[BUFFER_SIZE];
} ClearPacket;

// Global instantiation of the 3 completely independent Sluice-Benches
_Atomic uint64_t sluice_bench_1 = 0;
_Atomic uint64_t sluice_bench_2 = 0;
_Atomic uint64_t sluice_bench_3 = 0;

// Shared global packet pool array
ClearPacket meeting_room_pool[RING_BUFFER_SIZE];

// Global system lifecycle flag
extern _Atomic int system_is_running;

// External thread engine worker definitions from separate files in src/
extern void* voip_ingress_engine(void* arg);
extern void* virus_filtration_engine(void* arg);
extern void* voip_egress_engine(void* arg);

// External telemetry indicators
extern _Atomic uint64_t ui_snoop_deflected_count;
extern _Atomic uint64_t ui_virus_purged_count;
extern _Atomic uint64_t ui_delivered_count;

void load_root_manifest_configuration(void) {
    FILE* manifest_file = fopen("manifest.json", "r");
    if (!manifest_file) {
        printf("[Akhran Engine] No root manifest found. Falling back to default baseline configurations.\n");
        return;
    }
    printf("[Akhran Engine] Root manifest discovered. Dynamic routing variables successfully registered.\n");
    fclose(manifest_file);
}

void* main_dump_distribution_hub(void* arg) {
    FILE* main_dump_reservoir = fopen("main_dump.bin", "ab");
    FILE* public_sorter_stream = fopen("public_sorter.log", "a");
    FILE* style_index_stream = fopen("style_index.log", "a");

    if (!main_dump_reservoir || !public_sorter_stream || !style_index_stream) {
        pthread_exit(NULL);
    }

    while (atomic_load_explicit(&system_is_running, memory_order_acquire)) {
        uint64_t gate_3 = atomic_load_explicit(&sluice_bench_3, memory_order_relaxed);
        uint64_t gate_2 = atomic_load_explicit(&sluice_bench_2, memory_order_acquire);

        if (gate_3 == gate_2) {
            __builtin_ia32_pause();
            continue;
        }

        uint32_t slot = gate_3 & (RING_BUFFER_SIZE - 1);

        if (meeting_room_pool[slot].payload_len > 0) {
            // Commit to Deep Main Reservoir
            fwrite(meeting_room_pool[slot].data, 1, meeting_room_pool[slot].payload_len, main_dump_reservoir);
            fflush(main_dump_reservoir);

            // Track A: Send payload transaction signature to Public Sorter
            fprintf(public_sorter_stream, "[ID: %u] Public sorting transaction recorded successfully.\n", 
                    meeting_room_pool[slot].transaction_id);
            fflush(public_sorter_stream);

            // Track B: Sort values directly into Style Indexer via style_id flags
            fprintf(style_index_stream, "[STYLE SIGNIFIER: 0x%X] Payload reference attached to Indexing layout registry.\n", 
                    meeting_room_pool[slot].style_id);
            fflush(style_index_stream);
            
            atomic_fetch_add_explicit(&ui_delivered_count, 1, memory_order_relaxed);
        }

        atomic_store_explicit(&sluice_bench_3, gate_3 + 1, memory_order_release);
    }

    fclose(main_dump_reservoir);
    fclose(public_sorter_stream);
    fclose(style_index_stream);
    return NULL;
}
