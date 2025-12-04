#pragma once

#include "generated/component_block.grpc.pb.h"
#include "component_block_helper.h"

#include <grpcpp/grpcpp.h>
#include <memory>

class AsyncComponentBlockService {
 public:
  explicit AsyncComponentBlockService(std::shared_ptr<ComponentBlockHelper> helper);
  ~AsyncComponentBlockService();

  component::block::Api::AsyncService* service() { return &service_; }

  void Start(grpc::ServerCompletionQueue* cq, int initial_slots = 4);
  void Shutdown();

 private:
  class CallBase;
  class RegisterPowerMaxCall;
  class RegisterPowerStoreCall;
  class ListArraysCall;

  component::block::Api::AsyncService service_;
  grpc::ServerCompletionQueue* cq_;
  std::shared_ptr<ComponentBlockHelper> helper_;
};

