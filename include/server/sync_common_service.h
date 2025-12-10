#pragma once

#include "generated/common.grpc.pb.h"
#include "common_helper.h"
#include <memory>

namespace data_shield::_sync {

class CommonService : public common::Api::Service
{
public:
  explicit CommonService(std::shared_ptr<CommonHelper> helper);
  ~CommonService() override = default;

  grpc::Status ListApis(grpc::ServerContext *context, const common::ListApisInput *request,
                        common::ListApisOutput *response) override;

  grpc::Status GetSystemInfo(grpc::ServerContext *context, const common::SystemInfoInput *request,
                              common::SystemInfo *response) override;

private:
  std::shared_ptr<CommonHelper> helper_;
};

} // namespace data_shield::_sync
