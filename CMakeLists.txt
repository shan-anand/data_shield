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
file(GLOB SRC_UTIL src/util/*.cpp)
file(GLOB SRC_SERVICE src/service/*.cpp)
file(GLOB SRC_STUB src/stub/*.cpp)
file(GLOB SRC_ROOT src/*.cpp)

# common files
set(SRC_COMMON
  ${SRC_GENERATED}
  ${SRC_UTIL}
)

# CLI files for both server and client
set(SRC_CLI
  ${SRC_SERVICE}
  ${SRC_STUB}
  ${SRC_ROOT}
)

# Create object library - compiled once, linked to both executables
add_library(obj_common OBJECT ${SRC_COMMON})

add_executable(data_shield
  ${SRC_CLI}
  $<TARGET_OBJECTS:obj_common>
)

target_link_libraries(data_shield PRIVATE gRPC::grpc++ protobuf::libprotobuf Threads::Threads)

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
