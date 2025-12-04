#include "client/component_block_client.h"

using grpc::ClientContext;
using grpc::Status;

ComponentBlockClient::ComponentBlockClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(component::block::Api::NewStub(channel)) {}

bool ComponentBlockClient::RegisterPowerMax(const component::block::RegisterPowerMaxInput& in,
                                            component::block::RegisterArrayOutput* out) {
  ClientContext ctx;
  Status s = stub_->RegisterPowerMax(&ctx, in, out);
  return s.ok();
}

bool ComponentBlockClient::RegisterPowerStore(const component::block::RegisterPowerStoreInput& in,
                                              component::block::RegisterArrayOutput* out) {
  ClientContext ctx;
  Status s = stub_->RegisterPowerStore(&ctx, in, out);
  return s.ok();
}

bool ComponentBlockClient::ListArrays(const component::block::ListArraysInput& in,
                                      component::block::ListArraysOutput* out) {
  ClientContext ctx;
  Status s = stub_->ListArrays(&ctx, in, out);
  return s.ok();
}