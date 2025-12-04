#!/bin/bash

####### C++ code generation
# Ensure to create the generated directories
mkdir ../src/generated ../include/generated 2>/dev/null
# generate c++ code
protoc --proto_path=./ \
       --cpp_out=../src/generated \
       --grpc_out=../src/generated \
       --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
       common.proto component_block.proto
# move the headers to include/generated/
mv ../src/generated/*.h ../include/generated/
# Fix #include to use generated/ as prefix
sed -i 's/#include "\([^"]*\)\.pb\.h"/#include "generated\/\1.pb.h"/g' ../src/generated/*.cc

####### Python code generation
# --pyi_out is needed to create intellisense info. Without this intellisense won't work in the editor
mkdir -p ../python/data_shield 2>/dev/null
protoc --proto_path=./ \
       --python_out=../python/data_shield \
       --pyi_out=../python/data_shield \
       --grpc_out=../python/data_shield \
       --plugin=protoc-gen-grpc=`which grpc_python_plugin` \
       common.proto component_block.proto

# fix imports in generated Python gRPC files
sed -i 's/^import \([a-zA-Z_][a-zA-Z0-9_]*\)_pb2 as/from . import \1_pb2 as/g' ../python/data_shield/*_pb2_grpc.py
