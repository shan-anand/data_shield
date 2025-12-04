```markdown
# data_shield
Performs data protection of files and blocks to and from the cloud

This workspace implements gRPC services with UpperCamelCase RPC names:
- common.Api: ListApis, GetSystemInfo
- component.block.Api: RegisterPowerMax, RegisterPowerStore, ListArrays

## Project Structure

### Proto Definitions
- `proto/common.proto` — Common API service definitions
- `proto/component_block.proto` — Block component service for array management
- `proto/generate_grpc.sh` — Code generation script

### C++ Implementation
- `include/server/` — Server headers (sync/async services, helpers)
- `src/server/` — Server implementations
- `include/client/` — Client headers
- `src/client/` — Client implementations
- `src/main.cpp` — Main executable with server/client modes
- `include/generated/`, `src/generated/` — Generated protobuf/gRPC code

### Python Client
- `python/client.py` — Python client implementation
- `python/generated/` — Generated Python protobuf/gRPC modules

### Build System
- `CMakeLists.txt` — CMake configuration
- `clean.sh` — Cleanup script for build and generated files

## Build
```bash
mkdir build && cd build
cmake ..
make
```

## Usage

### Server
```bash
# Synchronous server
./data_shield --server-sync [address]

# Asynchronous server
./data_shield --server-async [address] [poller_threads] [initial_slots_per_rpc]
```
Defaults: address=0.0.0.0:50051, poller_threads=4, initial_slots_per_rpc=4

### C++ Client
```bash
./data_shield --client [address] <command> [args...]
```

### Python Client
```bash
./python/client.py --address [address] <command> [args...]
```

### Client Commands
- `list` — List available APIs
- `sysinfo` — Get system information (uptime, memory)
- `add-array-powermax <name> <symmetrix_id> <dns> <user> <pass>` — Register PowerMax array
- `add-array-powerstore <name> <appliance_id> <dns> <user> <pass>` — Register PowerStore array
- `list-arrays` — List registered arrays
- `count` — Count registered arrays
- `ping [msg]` — Health check with optional message
```
