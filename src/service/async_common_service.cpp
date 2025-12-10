#include "service/async_common_service.h"
#include "service/async_call_base.h"

#include <google/protobuf/empty.pb.h>

using common::Api;
using grpc::ServerCompletionQueue;
using namespace data_shield::_async;

// ICall for common service
class CommonService::ICall : public CallBase
{
public:
  ICall(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<CommonHelper> helper)
      : CallBase(cq), service_(service), helper_(helper) {}

protected:
  Api::AsyncService *service_;
  std::shared_ptr<CommonHelper> helper_;
};

// ListApis
class CommonService::ListApisCall : public CommonService::ICall
{
public:
  ListApisCall(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<CommonHelper> helper)
      : ICall(service, cq, helper), responder_(&ctx_)
  {
    Proceed();
  }

  static void Create(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<CommonHelper> helper)
  {
    new ListApisCall(service, cq, helper);
  }

protected:
  void RequestRPC() override
  {
    // RPC name in proto: ListApis -> generated AsyncService method is RequestListApis
    service_->RequestListApis(&ctx_, &request_, &responder_, cq_, cq_, this);
  }

  void rearm() override { Create(service_, cq_, helper_); }

  void OnProcess() override
  {
    common::ListApisOutput out = helper_->ListApis(request_);
    responder_.Finish(out, grpc::Status::OK, this);
  }

private:
  common::ListApisInput request_;
  grpc::ServerAsyncResponseWriter<common::ListApisOutput> responder_;
};

// GetSystemInfo
class CommonService::GetSystemInfoCall : public CommonService::ICall
{
public:
  GetSystemInfoCall(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<CommonHelper> helper)
      : ICall(service, cq, helper), responder_(&ctx_)
  {
    Proceed();
  }

  static void Create(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<CommonHelper> helper)
  {
    new GetSystemInfoCall(service, cq, helper);
  }

protected:
  void RequestRPC() override
  {
    // RPC name in proto: GetSystemInfo -> generated AsyncService method is RequestGetSystemInfo
    service_->RequestGetSystemInfo(&ctx_, &request_, &responder_, cq_, cq_, this);
  }

  void rearm() override { Create(service_, cq_, helper_); }

  void OnProcess() override
  {
    common::SystemInfo info = helper_->GetSystemInfo(request_);
    responder_.Finish(info, grpc::Status::OK, this);
  }

private:
  common::SystemInfoInput request_;
  grpc::ServerAsyncResponseWriter<common::SystemInfo> responder_;
};

// CommonService
CommonService::CommonService(std::shared_ptr<CommonHelper> helper)
    : service_(), cq_(nullptr), helper_(helper) {}

CommonService::~CommonService() {}

void CommonService::Start(ServerCompletionQueue *cq, int initial_slots)
{
  cq_ = cq;
  for (int i = 0; i < initial_slots; ++i)
  {
    ListApisCall::Create(&service_, cq, helper_);
    GetSystemInfoCall::Create(&service_, cq, helper_);
  }
}

void CommonService::Shutdown()
{
  // nothing specific; CallData cleanup happens on completion/cancel paths
}
