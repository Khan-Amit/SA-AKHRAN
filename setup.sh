#!/bin/bash
# SA-AKHRAN Automated Sluice Gate Infrastructure Setup Engine

echo "===================================================="
echo "          INITIALIZING SA-AKHRAN BACKEND             "
echo "===================================================="

# Ensure directories mirror the production thread layout
mkdir -p src hidden assets

# Check for required backend dependencies
if ! command -v gcc &> /dev/null; then
    echo "[!] Error: GCC compiler layer missing. Halting pipeline execution."
    exit 1
fi

echo "[*] Cleaning old build artifacts and purging redundant logs..."
rm -f engine_system *.bin *.log

echo "[*] Executing multi-file compiler serialization across memory lines..."
gcc main_coordinator.c src/state_controller.c src/file_one_voip_ingress.c src/file_two_filtration.c src/file_three_voip_egress.c -lpthread -O2 -o engine_system

if [ $? -eq 0 ]; then
    echo "[+] Pipeline compilation successful: 'engine_system' generated."
    echo "[+] Sluice Protected Guard successfully mounted to target environment."
    echo "===================================================="
    echo "To fire up the main installation engine UI, execute: ./engine_system"
else
    echo "[-] Error: Cross-compilation failed. Check hardware core atomic states."
    exit 1
fi
