#include "server/sync_common_service.h"

using grpc::ServerContext;
using grpc::Status;
using namespace data_shield::_sync;

CommonService::CommonService(std::shared_ptr<CommonHelper> helper)
    : helper_(helper) {}

Status CommonService::ListApis(ServerContext * /*context*/, const common::ListApisInput *request,
                               common::ListApisOutput *response)
{
  *response = helper_->ListApis(*request);
  return Status::OK;
}

Status CommonService::GetSystemInfo(ServerContext * /*context*/, const common::SystemInfoInput *request,
                                    common::SystemInfo *response)
{
  *response = helper_->GetSystemInfo(*request);
  return Status::OK;
}
