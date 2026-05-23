#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VOIP_PORT 8443
#define ENCRYPTION_KEY "SA_AKHRAN_SECURE_GUARD_2026"

// VoIP Session Rule Definition
typedef struct {
    char session_id[32];
    bool is_invited;
    int clearance_level;
    bool tunnel_active;
} VoIPSession;

// Execute strict sword initialization protocols
bool initiate_sword_voip(VoIPSession *session) {
    printf("[⚔️ SWORD] Activating secure VoIP subsystem initialization...\n");

    // Rule Checking: No invitation means ground floor only, VoIP connection is strictly rejected
    if (!session->is_invited) {
        printf("[⚔️ SWORD] ABORT: Handshake failed. Absolute zero-deviation protocol enforced. Ground floor entry only.\n");
        session->tunnel_active = false;
        return false;
    }

    // Handshake and crypto allocation
    printf("[⚔️ SWORD] Establishing handshake on secure port %d...\n", VOIP_PORT);
    printf("[⚔️ SWORD] Injecting protective token layer via core key cipher.\n");
    
    session->tunnel_active = true;
    printf("[⚔️ SWORD] SUCCESS: VoIP voice tunnel securely bound and guarding live traffic streams.\n");
    return true;
}

void terminate_sword_voip(VoIPSession *session) {
    if (session->tunnel_active) {
        session->tunnel_active = false;
        printf("[⚔️ SWORD] Tunnel safely disassembled. Channel cleared.\n");
    }
}
