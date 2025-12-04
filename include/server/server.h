#pragma once

#include <grpcpp/grpcpp.h>

#include "server/async_call_base.h"
#include "server/async_common_service.h"
#include "server/async_component_block_service.h"

#include "server/sync_common_service.h"
#include "server/sync_component_block_service.h"

class Server
{
public:
  Server();
  virtual ~Server();
  virtual void run_async();
  virtual void run_sync();

  std::string address; // Server address and port
  std::shared_ptr<grpc::ServerCredentials> creds; // Server credentials
  int poller_threads; // used only for run_async()
  int initial_slots;  // used only for run_async()
};
