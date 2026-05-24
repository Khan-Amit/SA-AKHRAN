#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdatomic.h>

#define VOIP_PORT 5060
#define BUFFER_SIZE 4096
#define RING_BUFFER_SIZE 16 // Must be power of 2 for fast masking

// Style & Payload Packet Structure
typedef struct {
    uint32_t style_id;
    uint32_t sequence;
    size_t payload_len;
    char data[BUFFER_SIZE];
} AudioPacket;

// The Three Sluice-Gate Control Mechanics (Atomic Fences)
typedef struct {
    _Atomic uint64_t gate_1_protect_front; // Protector 1: Validates incoming stream integrity
    _Atomic uint64_t gate_2_protect_back;  // Protector 2: Guards memory overwrites from fast threads
    _Atomic uint64_t gate_3_deliver;       // Deliverer 3: Validates steady-state processing consumption
} SluiceGateController;

static AudioPacket ring_buffer[RING_BUFFER_SIZE];
static SluiceGateController sluice_gates = {0, 0, 0};

// --- SLUICE GATE 1 & 2: STREAM RECEPTION AND PROTECTION ---
void* voip_stream_listener(void* arg) {
    int server_fd;
    struct sockaddr_in address;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        pthread_exit(NULL);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(VOIP_PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd);
        pthread_exit(NULL);
    }

    while (1) {
        uint64_t current_front = atomic_load_explicit(&sluice_gates.gate_1_protect_front, memory_order_relaxed);
        uint64_t current_deliver = atomic_load_explicit(&sluice_gates.gate_3_deliver, memory_order_acquire);

        // Nanosecond Check: Sluice-Bench Protector 2 prevents overtaking delivery ring
        if ((current_front - current_deliver) >= RING_BUFFER_SIZE) {
            // Buffer is temporarily full; force processor backoff to allow delivery thread to catch up
            __builtin_ia32_pause(); 
            continue;
        }

        uint32_t index = current_front & (RING_BUFFER_SIZE - 1);
        
        // Temporary holding buffer for Sluice Gate 1 inspection
        char raw_buffer[BUFFER_SIZE + 8]; 
        ssize_t bytes_recv = recvfrom(server_fd, raw_buffer, sizeof(raw_buffer), 0, 
                                      (struct sockaddr*)&client_addr, &addr_len);
        
        if (bytes_recv > 8) {
            // Sluice-Gate Protector 1: Extract Style Layer Headers & Metadata
            memcpy(&ring_buffer[index].style_id, raw_buffer, 4);
            memcpy(&ring_buffer[index].sequence, raw_buffer + 4, 4);
            
            ring_buffer[index].payload_len = bytes_recv - 8;
            memcpy(ring_buffer[index].data, raw_buffer + 8, ring_buffer[index].payload_len);

            // Sluice-Gate Protector 2: Commit memory ordering across cpu cores securely
            atomic_store_explicit(&sluice_gates.gate_2_protect_back, current_front + 1, memory_order_release);
            atomic_store_explicit(&sluice_gates.gate_1_protect_front, current_front + 1, memory_order_release);
        }
    }

    close(server_fd);
    return NULL;
}

// --- SLUICE GATE 3: ENGINE DELIVERY & DUMPING LAYER (`engine.c`) ---
void* engine_delivery_pipeline(void* arg) {
    FILE* dump_file = fopen("stream_dump.bin", "ab");
    FILE* style_log = fopen("style_collection.log", "a");

    if (!dump_file || !style_log) {
        pthread_exit(NULL);
    }

    while (1) {
        uint64_t current_deliver = atomic_load_explicit(&sluice_gates.gate_3_deliver, memory_order_relaxed);
        uint64_t current_back = atomic_load_explicit(&sluice_gates.gate_2_protect_back, memory_order_acquire);

        // Nanosecond Check: Sluice-Bench Deliverer 3 verifies if raw data is ready to release
        if (current_deliver == current_back) {
            __builtin_ia32_pause(); // Nanosecond pause loop if queue is empty
            continue;
        }

        uint32_t index = current_deliver & (RING_BUFFER_SIZE - 1);

        // Stream Collection & Proper Mechanical File Dump
        fwrite(ring_buffer[index].data, 1, ring_buffer[index].payload_len, dump_file);
        fprintf(style_log, "[SEQ: %u] Extracted Style Signifier: 0x%X\n", 
                ring_buffer[index].sequence, ring_buffer[index].style_id);

        // Prevent slow-write file-system buffering delays from causing nanosecond pipeline drops
        fflush(dump_file);
        fflush(style_log);

        // Advance Deliverer 3 gate past the consumed index
        atomic_store_explicit(&sluice_gates.gate_3_deliver, current_deliver + 1, memory_order_release);
    }

    fclose(dump_file);
    fclose(style_log);
    return NULL;
}

int init_complete_pipeline() {
    pthread_t listener_id, engine_id;
    
    if (pthread_create(&listener_id, NULL, voip_stream_listener, NULL) != 0) return -1;
    if (pthread_create(&engine_id, NULL, engine_delivery_pipeline, NULL) != 0) return -1;

    pthread_detach(listener_id);
    pthread_detach(engine_id);
    return 0;
}
