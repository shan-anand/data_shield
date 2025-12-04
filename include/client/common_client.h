#pragma once

#include "generated/common.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

using grpc::Channel;

class CommonClient {
 public:
  explicit CommonClient(std::shared_ptr<Channel> channel);

  // Synchronous calls
  bool ListApis(const std::string& search, common::ListApisOutput* out);
  bool GetSystemInfo(common::SystemInfo* out);

 private:
  std::unique_ptr<common::Api::Stub> stub_;
};

