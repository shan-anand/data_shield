#pragma once

#include "generated/component_block.grpc.pb.h"
#include "component_block_helper.h"
#include <memory>

class ComponentBlockServiceImpl final : public component::block::Api::Service {
 public:
  explicit ComponentBlockServiceImpl(std::shared_ptr<ComponentBlockHelper> helper);
  ~ComponentBlockServiceImpl() override = default;

  grpc::Status RegisterPowerMax(grpc::ServerContext* context,
                                const component::block::RegisterPowerMaxInput* request,
                                component::block::RegisterArrayOutput* response) override;

  grpc::Status RegisterPowerStore(grpc::ServerContext* context,
                                  const component::block::RegisterPowerStoreInput* request,
                                  component::block::RegisterArrayOutput* response) override;

  grpc::Status ListArrays(grpc::ServerContext* context,
                          const component::block::ListArraysInput* request,
                          component::block::ListArraysOutput* response) override;

 private:
  std::shared_ptr<ComponentBlockHelper> helper_;
};

