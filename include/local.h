#pragma once

#include <grpcpp/grpcpp.h>
#include "server/server.h"
#include "client/client.h"

#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>

namespace local
{
  void print_usage(const char* prog);
  int run_client(int argc, char** argv);

  void print(const common::Space &_space, std::string_view _prefix);
  void print(const google::protobuf::Timestamp &_timestamp);
  void print(const common::SystemInfo &systemInfo);
}
