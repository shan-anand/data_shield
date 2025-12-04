#include "server/component_block_service_impl.h"

using grpc::ServerContext;
using grpc::Status;
using component::block::RegisterArrayOutput;

ComponentBlockServiceImpl::ComponentBlockServiceImpl(std::shared_ptr<ComponentBlockHelper> helper)
    : helper_(helper) {}

Status ComponentBlockServiceImpl::RegisterPowerMax(ServerContext* /*context*/,
                                                   const component::block::RegisterPowerMaxInput* request,
                                                   component::block::RegisterArrayOutput* response) {
  auto info = helper_->RegisterPowerMax(*request);
  response->set_success(true);
  response->mutable_array()->CopyFrom(info);
  return Status::OK;
}

Status ComponentBlockServiceImpl::RegisterPowerStore(ServerContext* /*context*/,
                                                     const component::block::RegisterPowerStoreInput* request,
                                                     component::block::RegisterArrayOutput* response) {
  auto info = helper_->RegisterPowerStore(*request);
  response->set_success(true);
  response->mutable_array()->CopyFrom(info);
  return Status::OK;
}

Status ComponentBlockServiceImpl::ListArrays(ServerContext* /*context*/,
                                             const component::block::ListArraysInput* request,
                                             component::block::ListArraysOutput* response) {
  auto list = helper_->ListArrays(*request);
  for (const auto& a : list) {
    auto out = response->add_arrays();
    out->CopyFrom(a);
  }
  return Status::OK;
}