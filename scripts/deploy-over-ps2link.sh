set -e 

# Define some paths 
PROJECT_ROOT="/workspace" 
BUILD_DIR="$PROJECT_ROOT/build" # Where CMake builds the ELF and irx files 
ASSETS_DIR="$PROJECT_ROOT/assets" # Images and fonts 
PS2HOST="$PROJECT_ROOT/host" # We are going to use this as a fake "host" directory when operating ps2link 
PS2CLIENT_CMD="ps2client -h 192.168.0.10" 
echo "Using PS2 client command: $PS2CLIENT_CMD with host directory $PS2HOST"

# Remove any old ps2host folder, then create it. 
rm -rf "$PS2HOST"
mkdir -p "$PS2HOST"

# Create symlinks inside ps2host: 
cd "$PS2HOST"
ln -s "$BUILD_DIR" build
ln -s "$ASSETS_DIR" assets

# (Optional sanity check: list out the files just to be sure)
echo "Inside $PS2HOST, contents of build/ee:"
ls -l build/ee

# Reset the PS2 so it's in PS2Link mode 
echo "Resetting PS2 via ps2link"
$PS2CLIENT_CMD reset
sleep 2

# Finally, run the EE ELF 
cd "$PS2HOST"
echo "Launching EE executable on PS2 (this call will block until the PS2 program exits)…"
$PS2CLIENT_CMD execee host:build/ee/flightcomputer.elf

cd "$PROJECT_ROOT"
rm -rf "$PS2HOST" # Clean up the ps2host directory after we're done
echo "Deployment complete. The PS2 should now be running the flight computer application."
