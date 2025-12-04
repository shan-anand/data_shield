#include "client/common_client.h"

#include <google/protobuf/empty.pb.h>

using grpc::ClientContext;
using grpc::Status;

CommonClient::CommonClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(common::Api::NewStub(channel)) {}

bool CommonClient::ListApis(const std::string& search, common::ListApisOutput* out) {
  common::ListApisInput in;
  in.set_search(search);
  ClientContext ctx;
  Status s = stub_->ListApis(&ctx, in, out);
  return s.ok();
}

bool CommonClient::GetSystemInfo(common::SystemInfo* out) {
  google::protobuf::Empty in;
  ClientContext ctx;
  Status s = stub_->GetSystemInfo(&ctx, in, out);
  return s.ok();
}