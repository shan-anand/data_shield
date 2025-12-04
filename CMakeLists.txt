cmake_minimum_required(VERSION 3.30)
project(component_common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)

execute_process(
  COMMAND ./generate_grpc.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/proto
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SRC_ROOT src/*.cpp)
file(GLOB SRC_SERVER src/server/*.cpp)
file(GLOB SRC_CLIENT src/client/*.cpp)
file(GLOB SRC_GENERATED src/generated/*.cc)

set(SRC_COMMON
  ${SRC_ROOT}
  ${SRC_SERVER}
  ${SRC_CLIENT}
  ${SRC_GENERATED}
)

add_executable(data_shield
  ${SRC_COMMON}
)

target_link_libraries(data_shield PRIVATE gRPC::grpc++ protobuf::libprotobuf Threads::Threads)