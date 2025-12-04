#include "server/common_service_impl.h"

using grpc::ServerContext;
using grpc::Status;

CommonServiceImpl::CommonServiceImpl(std::shared_ptr<CommonHelper> helper)
    : helper_(helper) {}

Status CommonServiceImpl::ListApis(ServerContext* /*context*/, const common::ListApisInput* request,
                                  common::ListApisOutput* response) {
  *response = helper_->ListApis(*request);
  return Status::OK;
}

Status CommonServiceImpl::GetSystemInfo(ServerContext* /*context*/, const google::protobuf::Empty* /*request*/,
                                       common::SystemInfo* response) {
  *response = helper_->GetSystemInfo();
  return Status::OK;
}