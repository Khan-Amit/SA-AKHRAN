#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdatomic.h>
#include <fcntl.h>

#define INGRESS_PORT 5060
#define BUFFER_SIZE 4096
#define RING_BUFFER_SIZE 16

typedef struct {
    uint32_t transaction_id;
    uint32_t style_id;
    size_t payload_len;
    char data[BUFFER_SIZE];
} ClearPacket;

extern _Atomic int system_is_running;
extern _Atomic uint64_t sluice_bench_1;
extern _Atomic uint64_t sluice_bench_3;
extern ClearPacket meeting_room_pool[RING_BUFFER_SIZE];
extern _Atomic uint64_t ui_snoop_deflected_count;

void* voip_ingress_engine(void* arg) {
    int ingress_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in ingress_addr;
    
    int opt = 1;
    setsockopt(ingress_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    fcntl(ingress_fd, F_SETFL, O_NONBLOCK);

    memset(&ingress_addr, 0, sizeof(ingress_addr));
    ingress_addr.sin_family = AF_INET;
    ingress_addr.sin_addr.s_addr = INADDR_ANY;
    ingress_addr.sin_port = htons(INGRESS_PORT);
    bind(ingress_fd, (struct sockaddr*)&ingress_addr, sizeof(ingress_addr));

    char raw_flood_buffer[BUFFER_SIZE + 64];

    while (atomic_load_explicit(&system_is_running, memory_order_acquire)) {
        uint64_t gate_1 = atomic_load_explicit(&sluice_bench_1, memory_order_relaxed);
        uint64_t gate_3 = atomic_load_explicit(&sluice_bench_3, memory_order_acquire);

        if ((gate_1 - gate_3) >= RING_BUFFER_SIZE) {
            __builtin_ia32_pause();
            continue;
        }

        ssize_t bytes_collected = recvfrom(ingress_fd, raw_flood_buffer, sizeof(raw_flood_buffer), 0, NULL, NULL);
        if (bytes_collected <= 0) {
            __builtin_ia32_pause();
            continue;
        }

        // SLUICE-BENCH 1: Clear heavy debris and instantly deflect tracking seekers
        if (bytes_collected < 16 || strncmp(raw_flood_buffer, "AUTH_KEY", 8) != 0) {
            atomic_fetch_add_explicit(&ui_snoop_deflected_count, 1, memory_order_relaxed);
            continue; 
        }

        uint32_t target_slot = gate_1 & (RING_BUFFER_SIZE - 1);
        
        // Unpack headers safely into the global workspace memory structures
        memcpy(&meeting_room_pool[target_slot].transaction_id, raw_flood_buffer + 8, 4);
        memcpy(&meeting_room_pool[target_slot].style_id, raw_flood_buffer + 12, 4);
        
        meeting_room_pool[target_slot].payload_len = bytes_collected - 16;
        memcpy(meeting_room_pool[target_slot].data, raw_flood_buffer + 16, bytes_collected - 16);

        atomic_store_explicit(&sluice_bench_1, gate_1 + 1, memory_order_release);
    }
    
    close(ingress_fd);
    return NULL;
}
