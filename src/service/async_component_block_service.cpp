#include "service/async_component_block_service.h"
#include "service/async_call_base.h"

using component::block::Api;
using grpc::ServerCompletionQueue;
using namespace data_shield::_async;

void ComponentBlockService::Shutdown()
{
  // no-op
}

// ICall for component service
class ComponentBlockService::ICall : public CallBase
{
public:
  ICall(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<ComponentBlockHelper> helper)
      : CallBase(cq), service_(service), helper_(helper) {}

protected:
  Api::AsyncService *service_;
  std::shared_ptr<ComponentBlockHelper> helper_;
};

// RegisterPowerMax
class ComponentBlockService::RegisterPowerMaxCall : public ComponentBlockService::ICall
{
public:
  RegisterPowerMaxCall(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<ComponentBlockHelper> helper)
      : ICall(service, cq, helper), responder_(&ctx_)
  {
    Proceed();
  }

  static void Create(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<ComponentBlockHelper> helper)
  {
    new RegisterPowerMaxCall(service, cq, helper);
  }

protected:
  void RequestRPC() override
  {
    // RPC name in proto: RegisterPowerMax -> generated method is RequestRegisterPowerMax
    service_->RequestRegisterPowerMax(&ctx_, &request_, &responder_, cq_, cq_, this);
  }

  void rearm() override { Create(service_, cq_, helper_); }

  void OnProcess() override
  {
    component::block::RegisterArrayOutput out;
    auto info = helper_->RegisterPowerMax(request_);
    out.set_success(true);
    out.mutable_array()->CopyFrom(info);
    responder_.Finish(out, grpc::Status::OK, this);
  }

private:
  component::block::RegisterPowerMaxInput request_;
  grpc::ServerAsyncResponseWriter<component::block::RegisterArrayOutput> responder_;
};

// RegisterPowerStore
class ComponentBlockService::RegisterPowerStoreCall : public ComponentBlockService::ICall
{
public:
  RegisterPowerStoreCall(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<ComponentBlockHelper> helper)
      : ICall(service, cq, helper), responder_(&ctx_)
  {
    Proceed();
  }

  static void Create(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<ComponentBlockHelper> helper)
  {
    new RegisterPowerStoreCall(service, cq, helper);
  }

protected:
  void RequestRPC() override
  {
    // RPC name in proto: RegisterPowerStore -> generated method is RequestRegisterPowerStore
    service_->RequestRegisterPowerStore(&ctx_, &request_, &responder_, cq_, cq_, this);
  }

  void rearm() override { Create(service_, cq_, helper_); }

  void OnProcess() override
  {
    component::block::RegisterArrayOutput out;
    auto info = helper_->RegisterPowerStore(request_);
    out.set_success(true);
    out.mutable_array()->CopyFrom(info);
    responder_.Finish(out, grpc::Status::OK, this);
  }

private:
  component::block::RegisterPowerStoreInput request_;
  grpc::ServerAsyncResponseWriter<component::block::RegisterArrayOutput> responder_;
};

// ListArrays
class ComponentBlockService::ListArraysCall : public ComponentBlockService::ICall
{
public:
  ListArraysCall(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<ComponentBlockHelper> helper)
      : ICall(service, cq, helper), responder_(&ctx_)
  {
    Proceed();
  }

  static void Create(Api::AsyncService *service, ServerCompletionQueue *cq, std::shared_ptr<ComponentBlockHelper> helper)
  {
    new ListArraysCall(service, cq, helper);
  }

protected:
  void RequestRPC() override
  {
    // RPC name in proto: ListArrays -> generated method is RequestListArrays
    service_->RequestListArrays(&ctx_, &request_, &responder_, cq_, cq_, this);
  }

  void rearm() override { Create(service_, cq_, helper_); }

  void OnProcess() override
  {
    component::block::ListArraysOutput out;
    auto list = helper_->ListArrays(request_);
    for (const auto &a : list)
    {
      auto o = out.add_arrays();
      o->CopyFrom(a);
    }
    responder_.Finish(out, grpc::Status::OK, this);
  }

private:
  component::block::ListArraysInput request_;
  grpc::ServerAsyncResponseWriter<component::block::ListArraysOutput> responder_;
};

// ComponentBlockService
ComponentBlockService::ComponentBlockService(std::shared_ptr<ComponentBlockHelper> helper)
    : service_(), cq_(nullptr), helper_(helper) {}

ComponentBlockService::~ComponentBlockService() {}

void ComponentBlockService::Start(ServerCompletionQueue *cq, int initial_slots)
{
  cq_ = cq;
  for (int i = 0; i < initial_slots; ++i)
  {
    RegisterPowerMaxCall::Create(&service_, cq, helper_);
    RegisterPowerStoreCall::Create(&service_, cq, helper_);
    ListArraysCall::Create(&service_, cq, helper_);
  }
}
