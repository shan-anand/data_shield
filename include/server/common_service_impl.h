#pragma once

#include "generated/common.grpc.pb.h"
#include "common_helper.h"
#include <memory>

class CommonServiceImpl final : public common::Api::Service {
 public:
  explicit CommonServiceImpl(std::shared_ptr<CommonHelper> helper);
  ~CommonServiceImpl() override = default;

  // Note: RPC method names follow new UpperCamelCase convention.
  grpc::Status ListApis(grpc::ServerContext* context, const common::ListApisInput* request,
                        common::ListApisOutput* response) override;

  grpc::Status GetSystemInfo(grpc::ServerContext* context, const google::protobuf::Empty* request,
                             common::SystemInfo* response) override;

 private:
  std::shared_ptr<CommonHelper> helper_;
};

