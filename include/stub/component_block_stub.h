#pragma once

#include "generated/component_block.grpc.pb.h"

#include <grpcpp/grpcpp.h>
#include <memory>

using grpc::Channel;

namespace data_shield {

class ComponentBlockApiStub
{
public:
  explicit ComponentBlockApiStub(std::shared_ptr<Channel> channel);

  // Synchronous calls
  bool RegisterPowerMax(const component::block::RegisterPowerMaxInput& in,
                        component::block::RegisterArrayOutput* out);
  bool RegisterPowerStore(const component::block::RegisterPowerStoreInput& in,
                          component::block::RegisterArrayOutput* out);
  bool ListArrays(const component::block::ListArraysInput& in,
                  component::block::ListArraysOutput* out);

private:
  std::unique_ptr<component::block::Api::Stub> stub_;
};

} // namespace data_shield
