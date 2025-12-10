#include "stub/component_block_stub.h"

using grpc::ClientContext;
using grpc::Status;

using namespace data_shield;

ComponentBlockApiStub::ComponentBlockApiStub(std::shared_ptr<grpc::Channel> channel)
    : stub_(component::block::Api::NewStub(channel)) {}

bool ComponentBlockApiStub::RegisterPowerMax(const component::block::RegisterPowerMaxInput& in,
                                            component::block::RegisterArrayOutput* out) {
  ClientContext ctx;
  Status s = stub_->RegisterPowerMax(&ctx, in, out);
  return s.ok();
}

bool ComponentBlockApiStub::RegisterPowerStore(const component::block::RegisterPowerStoreInput& in,
                                              component::block::RegisterArrayOutput* out) {
  ClientContext ctx;
  Status s = stub_->RegisterPowerStore(&ctx, in, out);
  return s.ok();
}

bool ComponentBlockApiStub::ListArrays(const component::block::ListArraysInput& in,
                                      component::block::ListArraysOutput* out) {
  ClientContext ctx;
  Status s = stub_->ListArrays(&ctx, in, out);
  return s.ok();
}