#pragma once

#include <grpcpp/grpcpp.h>

#include "stub/common_stub.h"
#include "stub/component_block_stub.h"
#include "util/host_info.h"

namespace data_shield {

class Client
{
public:
  Client(const std::string& server_address);
  Client(const util::HostInfo& hostInfo);
  Client(std::shared_ptr<grpc::Channel> channel);

public:
  CommonApiStub common;
  ComponentBlockApiStub component_block;
  std::shared_ptr<grpc::Channel> channel() const { return channel_; }

private:
  std::shared_ptr<grpc::Channel> channel_;
};

} // namespace data_shield
