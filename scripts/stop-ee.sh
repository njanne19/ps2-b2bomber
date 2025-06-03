set -e

# 1) Locate the EE ELF
EE_ELF="./build/ee/flightcomputer.elf"
if [ ! -f "$EE_ELF" ]; then
  echo "Error: EE ELF not found at $EE_ELF"
  exit 1
fi

# 2) Extract the address of 'keep_running' from the symbol table
#    We look for a line containing "keep_running" and grab its value.
SYM_INFO=$(readelf -Ws "$EE_ELF" | grep " keep_running$")
if [ -z "$SYM_INFO" ]; then
  echo "Error: Symbol 'keep_running' not found in $EE_ELF"
  exit 1
fi

# Expected format (example):
#   1234: 0000000010012340     4 OBJECT  GLOBAL DEFAULT    2 keep_running
# We want the third column (the address). Use awk to grab it.
SYM_ADDR=$(echo "$SYM_INFO" | awk '{ print $2 }')
if [ -z "$SYM_ADDR" ]; then
  echo "Error: Unable to parse keep_running address from:"
  echo "$SYM_INFO"
  exit 1
fi

# Convert to 0x... format if needed (readelf gives hex without 0x prefix)
if [[ ! "$SYM_ADDR" =~ ^0x ]]; then
  SYM_ADDR="0x$SYM_ADDR"
fi

echo "Found keep_running at address: $SYM_ADDR"

# 3) Use ps2client to write zero to that address (4 bytes)
PS2CLIENT="ps2client -h 192.168.0.10"
echo "Sending writemem to set keep_running=0..."
$PS2CLIENT writemem "$SYM_ADDR" 4 0

echo "Done: keep_running has been cleared. If the EE was stuck in its loop, it should now exit."