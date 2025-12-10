#pragma once

#include <grpcpp/grpcpp.h>

#include "client/common_client.h"
#include "client/component_block_client.h"
#include "util/host_info.h"

namespace data_shield {

class Client
{
public:
  Client(const std::string& server_address);
  Client(const util::HostInfo& hostInfo);
  Client(std::shared_ptr<grpc::Channel> channel);

public:
  CommonClient common;
  ComponentBlockClient component_block;
  std::shared_ptr<grpc::Channel> channel() const { return channel_; }

private:
  std::shared_ptr<grpc::Channel> channel_;
};

} // namespace data_shield
