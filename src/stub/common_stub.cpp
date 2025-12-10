#include "stub/common_stub.h"

#include <google/protobuf/empty.pb.h>

using grpc::ClientContext;
using grpc::Status;

using namespace data_shield;

CommonApiStub::CommonApiStub(std::shared_ptr<grpc::Channel> channel)
    : stub_(common::Api::NewStub(channel)) {}

bool CommonApiStub::ListApis(const std::string& search, common::ListApisOutput* out) {
  common::ListApisInput in;
  in.set_search(search);
  ClientContext ctx;
  Status s = stub_->ListApis(&ctx, in, out);
  return s.ok();
}

bool CommonApiStub::GetSystemInfo(const common::SystemInfoInput& in, common::SystemInfo* out) {
  ClientContext ctx;
  Status s = stub_->GetSystemInfo(&ctx, in, out);
  return s.ok();
}