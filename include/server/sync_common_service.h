#pragma once

#include "generated/common.grpc.pb.h"
#include "common_helper.h"
#include <memory>

namespace _sync {

class CommonService : public common::Api::Service
{
public:
  explicit CommonService(std::shared_ptr<CommonHelper> helper);
  ~CommonService() override = default;

  grpc::Status ListApis(grpc::ServerContext *context, const common::ListApisInput *request,
                        common::ListApisOutput *response) override;

  grpc::Status GetSystemInfo(grpc::ServerContext *context, const google::protobuf::Empty *request,
                              common::SystemInfo *response) override;

private:
  std::shared_ptr<CommonHelper> helper_;
};

} // namespace _sync
