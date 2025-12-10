cmake_minimum_required(VERSION 3.30)
# Set compilers before project() command
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_LINKER_TYPE LLD)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -save-temps=obj -S -emit-llvm")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps=obj -S -emit-llvm")

project(component_common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Print compiler info
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler , linker: ${CMAKE_CXX_COMPILER} , ${CMAKE_LINKER_TYPE})")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")


find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)

execute_process(
  COMMAND ./generate_grpc.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/proto
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SRC_GENERATED src/generated/*.cc)
file(GLOB SRC_SERVICE src/service/*.cpp)
file(GLOB SRC_STUB src/stub/*.cpp)

# common files
set(SRC_COMMON
  ${SRC_GENERATED}
)

# cli common files
set(SRC_CLI_COMMON
  src/cli/args.cpp
  src/cli/host_info.cpp
  src/cli/uptime.cpp
)

# CLI files for server
set(SRC_DATA_SHIELD_SERVER
  ${SRC_SERVICE}
  src/cli/server.cpp
  src/cli/data_shield_server.cpp
)

# CLI files for client
set(SRC_DATA_SHIELD_CLIENT
  ${SRC_STUB}
  src/cli/client.cpp
  src/cli/data_shield_client.cpp
)

# Create object library - compiled once, linked to both executables
add_library(obj_common OBJECT ${SRC_COMMON})

# Create object library for cli - compiled once, linked to both executables
add_library(obj_cli_common OBJECT ${SRC_CLI_COMMON})

add_executable(data_shield_server
  ${SRC_DATA_SHIELD_SERVER}
  $<TARGET_OBJECTS:obj_common>
  $<TARGET_OBJECTS:obj_cli_common>
)

add_executable(data_shield_client
  ${SRC_DATA_SHIELD_CLIENT}
  $<TARGET_OBJECTS:obj_common>
  $<TARGET_OBJECTS:obj_cli_common>
)

target_link_libraries(data_shield_server PRIVATE gRPC::grpc++ protobuf::libprotobuf Threads::Threads)
target_link_libraries(data_shield_client PRIVATE gRPC::grpc++ protobuf::libprotobuf Threads::Threads)

# Enable testing support
enable_testing()

# Run the server
add_test(NAME StartServer
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/data_shield
  --server --background
)
set_tests_properties(StartServer PROPERTIES
    FIXTURES_SETUP SERVER_FIXTURE
    TIMEOUT 3
)

# Run the client tests
add_test(NAME SystemInfo
  COMMAND data_shield
  --client
  sysinfo
)
set_tests_properties(SystemInfo PROPERTIES
    FIXTURES_REQUIRED SERVER_FIXTURE
)
