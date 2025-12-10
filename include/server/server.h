#pragma once

#include <syslog.h>

#include <grpcpp/grpcpp.h>

#include "server/async_call_base.h"
#include "server/async_common_service.h"
#include "server/async_component_block_service.h"

#include "server/sync_common_service.h"
#include "server/sync_component_block_service.h"
#include "util/host_info.h"

namespace data_shield {

class Server
{
public:
  enum class RunMode { Sync, Async };
public:
  Server();
  Server(const util::HostInfo& _hostInfo);
  virtual ~Server();
  void run();

  util::HostInfo  hostInfo;       // Host and port info
  RunMode         runMode;        // run mode (sync or async)
  bool            background;     // run in background
  std::shared_ptr<grpc::ServerCredentials> creds; // Server credentials
  int             poller_threads; // used only for run_async()
  int             initial_slots;  // used only for run_async()

// The following functions have a default implementation but can be overridden by subclasses
protected:
  virtual void run_async();
  virtual void run_sync();
};

} // namespace data_shield
