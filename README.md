```markdown
# data_shield
Performs data protection of files and blocks to and from the cloud

This workspace has been regenerated so RPC names follow UpperCamelCase:
- common.Api: ListApis, GetSystemInfo
- component.block.Api: RegisterPowerMax, RegisterPowerStore, ListArrays

This avoids surprises in generated method names for async servers (RequestListApis instead of RequestlistApis).

Files:
- common.proto, component_block.proto — proto definitions (updated RPC names)
- CMakeLists.txt — runs protoc + grpc_cpp_plugin to generate C++ sources into build/generated
- array_store.{h,cpp}, system_info.{h,cpp} — small in-memory/state providers
- common_service_impl.{h,cpp}, component_block_service_impl.{h,cpp} — synchronous service implementations
- async_common_service.{h,cpp}, async_component_block_service.{h,cpp} — async service handlers that use AsyncCallBase
- async_call_base.h — common base for CallData
- client.{h,cpp} — simple synchronous client wrappers
- main.cpp — single executable supporting --server-sync, --server-async and --client modes

Build
- mkdir build && cd build
- cmake ..
- make

Run examples
- Start synchronous server:
  ./data_shield --server-sync 0.0.0.0:50051

- Start async server:
  ./data_shield --server-async 0.0.0.0:50051 4 4

- Client examples:
  ./data_shield --client localhost:50051 list
  ./data_shield --client localhost:50051 sysinfo
  ./data_shield --client localhost:50051 add-array-powermax MyArray sx-1 dns user pass
```
