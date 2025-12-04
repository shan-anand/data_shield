#pragma once

#include "generated/common.grpc.pb.h"
#include "common_helper.h"

#include <grpcpp/grpcpp.h>
#include <memory>

class AsyncCommonService {
 public:
  explicit AsyncCommonService(std::shared_ptr<CommonHelper> helper);
  ~AsyncCommonService();

  common::Api::AsyncService* service() { return &service_; }

  void Start(grpc::ServerCompletionQueue* cq, int initial_slots = 2);
  void Shutdown();

 private:
  class CallBase;
  class ListApisCall;
  class GetSystemInfoCall;

  common::Api::AsyncService service_;
  grpc::ServerCompletionQueue* cq_;
  std::shared_ptr<CommonHelper> helper_;
};

