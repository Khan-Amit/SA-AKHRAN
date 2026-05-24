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
    uint32_t style_id; // Metadata flag used by the Indexer
    size_t payload_len;
    char data[BUFFER_SIZE];
} ClearPacket;

// Global instantiation of the 3 independent Sluice-Benches
_Atomic uint64_t sluice_bench_1 = 0;
_Atomic uint64_t sluice_bench_2 = 0;
_Atomic uint64_t sluice_bench_3 = 0;

// Central pool shared across the decoupled files
ClearPacket meeting_room_pool[RING_BUFFER_SIZE];

// External thread engine functions from your separate files
extern void* voip_ingress_engine(void* arg);
extern void* virus_filtration_engine(void* arg);
extern void* voip_egress_engine(void* arg);

// --- FILTRATION TO RESERVOIR ROUTING (MAIN DUMP & DISTRIBUTION) ---
void* main_dump_distribution_hub(void* arg) {
    FILE* main_dump_reservoir = fopen("main_dump.bin", "ab");
    FILE* public_sorter_stream = fopen("public_sorter.log", "a");
    FILE* style_index_stream = fopen("style_index.log", "a");

    if (!main_dump_reservoir || !public_sorter_stream || !style_index_stream) {
        pthread_exit(NULL);
    }

    while (1) {
        uint64_t gate_3 = atomic_load_explicit(&sluice_bench_3, memory_order_relaxed);
        uint64_t gate_2 = atomic_load_explicit(&sluice_bench_2, memory_order_acquire);

        // Await next entry processed by Sluice-Bench 2
        if (gate_3 == gate_2) {
            __builtin_ia32_pause();
            continue;
        }

        uint32_t slot = gate_3 & (RING_BUFFER_SIZE - 1);

        if (meeting_room_pool[slot].payload_len > 0) {
            // 1. Commit raw incoming data immediately to the Main Dump Reservoir
            fwrite(meeting_room_pool[slot].data, 1, meeting_room_pool[slot].payload_len, main_dump_reservoir);
            fflush(main_dump_reservoir);

            // 2. Track A: Public Sorter Distribution
            fprintf(public_sorter_stream, "[ID: %u] Public sorting transaction executed.\n", 
                    meeting_room_pool[slot].transaction_id);
            fflush(public_sorter_stream);

            // 3. Track B: Style Indexing Distribution
            fprintf(style_index_stream, "[STYLE: 0x%X] Mapped payload to Index layer.\n", 
                    meeting_room_pool[slot].style_id);
            fflush(style_index_stream);
        }

        // Release to Sluice-Bench 3 for final untrackable exit delivery
        atomic_store_explicit(&sluice_bench_3, gate_3 + 1, memory_order_release);
    }

    fclose(main_dump_reservoir);
    fclose(public_sorter_stream);
    fclose(style_index_stream);
    return NULL;
}

int main(void) {
    pthread_t ingress_id;
    pthread_t filtration_id;
    pthread_t distribution_id;
    pthread_t egress_id;

    // Fire up the completely isolated engine layers
    pthread_create(&ingress_id, NULL, voip_ingress_engine, NULL);
    pthread_create(&filtration_id, NULL, virus_filtration_engine, NULL);
    pthread_create(&distribution_id, NULL, main_dump_distribution_hub, NULL);
    pthread_create(&egress_id, NULL, voip_egress_engine, NULL);

    // Keep the core processing system alive
    pthread_join(ingress_id, NULL);
    pthread_join(filtration_id, NULL);
    pthread_join(distribution_id, NULL);
    pthread_join(egress_id, NULL);

    return 0;
}
