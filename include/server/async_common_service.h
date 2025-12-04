#pragma once

#include "generated/common.grpc.pb.h"
#include "common_helper.h"

#include <grpcpp/grpcpp.h>
#include <memory>

namespace _async {

class CommonService
{
public:
  explicit CommonService(std::shared_ptr<CommonHelper> helper);
  ~CommonService();

  common::Api::AsyncService *service() { return &service_; }

  void Start(grpc::ServerCompletionQueue *cq, int initial_slots = 2);
  void Shutdown();

private:
  class ICall;
  class ListApisCall;
  class GetSystemInfoCall;

  common::Api::AsyncService service_;
  grpc::ServerCompletionQueue *cq_;
  std::shared_ptr<CommonHelper> helper_;
};

} // namespace async
