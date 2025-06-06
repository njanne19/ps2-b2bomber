set -e 

# Define some paths 
PROJECT_ROOT="/workspace" 
BUILD_DIR="$PROJECT_ROOT/build" # Where CMake builds the ELF and irx files 
ASSETS_DIR="$PROJECT_ROOT/assets" # Images and fonts 
PS2CLIENT_CMD="ps2client -h 192.168.0.10" 

# Run the EE ELF 
echo "Launching EE executable on PS2 (this call will block)…"
$PS2CLIENT_CMD execee host:build/ee/flightcomputer.elf
