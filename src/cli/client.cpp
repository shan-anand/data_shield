
#include "cli/client.h"

using namespace data_shield::cli;

Client::Client(const std::string& server_address) :
  Client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()))
{}

Client::Client(const HostInfo& hostInfo) :
  Client(hostInfo.to_str())
{}

Client::Client(std::shared_ptr<grpc::Channel> channel) :
  channel_(channel),
  common(channel),
  component_block(channel)
{}
