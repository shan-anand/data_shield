#pragma once

#include "generated/component_block.grpc.pb.h"
#include "component_block_helper.h"

#include <grpcpp/grpcpp.h>
#include <memory>

namespace _async {

class ComponentBlockService
{
public:
  explicit ComponentBlockService(std::shared_ptr<ComponentBlockHelper> helper);
  ~ComponentBlockService();

  component::block::Api::AsyncService *service() { return &service_; }

  void Start(grpc::ServerCompletionQueue *cq, int initial_slots = 4);
  void Shutdown();

private:
  class ICall;
  class RegisterPowerMaxCall;
  class RegisterPowerStoreCall;
  class ListArraysCall;

  component::block::Api::AsyncService service_;
  grpc::ServerCompletionQueue *cq_;
  std::shared_ptr<ComponentBlockHelper> helper_;
};

} // namespace async
