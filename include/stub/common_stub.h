#pragma once

#include "generated/common.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

using grpc::Channel;

namespace data_shield {

class CommonApiStub
{
public:
  explicit CommonApiStub(std::shared_ptr<Channel> channel);

  // Synchronous calls
  bool ListApis(const std::string& search, common::ListApisOutput* out);
  bool GetSystemInfo(const common::SystemInfoInput& in, common::SystemInfo* out);

private:
  std::unique_ptr<common::Api::Stub> stub_;
};

} // namespace data_shield
