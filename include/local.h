#pragma once

#include <grpcpp/grpcpp.h>

#include "server/common_service_impl.h"
#include "server/component_block_service_impl.h"
#include "server/async_common_service.h"
#include "server/async_component_block_service.h"
#include "server/async_call_base.h"
#include "server/common_helper.h"
#include "server/component_block_helper.h"

#include "client/client.h"

#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;

namespace local
{
  void print_usage(const char* prog);

  void run_server_sync(const std::string& server_address);
  void run_server_async(const std::string& server_address, int poller_threads, int initial_slots);
  int run_client(int argc, char** argv);

  void print(const common::Space &_space, std::string_view _prefix);
  void print(const google::protobuf::Timestamp &_timestamp);
  void print(const common::SystemInfo &systemInfo);
}
