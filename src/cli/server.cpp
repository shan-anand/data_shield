#include "cli/server.h"

#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

using namespace std;
using namespace data_shield::cli;

Server::Server(const HostInfo& _hostInfo) :
  hostInfo(_hostInfo),
  runMode(RunMode::Sync),
  background(false),
  creds(grpc::InsecureServerCredentials()),
  poller_threads(4),
  initial_slots(4)
{
}

Server::Server() : Server(HostInfo())
{
}

Server::~Server()
{
}

/*static*/
Server::RunMode Server::getRunMode(const std::string& _modeStr)
{
  const char* p = (_modeStr.substr(0, 2) != "--")? _modeStr.c_str() : _modeStr.c_str() + 2;
  std::string_view modeViewStr(p);
  if ( modeViewStr == "async" )
    return RunMode::Async;
  else if ( modeViewStr == "sync" )
    return RunMode::Sync;
  else
    throw std::invalid_argument("Invalid run mode string: " + _modeStr);
}

void Server::run()
{
  auto run_here = [&]() {
    if ( this->runMode == RunMode::Async )
      this->run_async();
    else
      this->run_sync();
  };

  if ( !this->hostInfo )
    throw std::invalid_argument("Empty host address");

  // if not background, run in this process here
  if ( !this->background )
    return run_here();

  pid_t childPid = fork();
  if ( childPid == -1 )
    throw std::runtime_error("Failed to fork background process");

if ( childPid == 0 )
  {
    // This is the child process, close the parent's file descriptors
    ::setsid();
    /*
    int max_fd = sysconf(_SC_OPEN_MAX);
    for ( int fd = 3; fd < max_fd; ++fd )
      ::close(fd);
    */

    int dev_null = ::open("/dev/null", O_WRONLY);
    if ( dev_null >= 0 )
    {
      ::dup2(dev_null, STDOUT_FILENO);
      ::dup2(dev_null, STDERR_FILENO);
      if ( dev_null > STDERR_FILENO )
        ::close(dev_null);
    }
    run_here();
  }
}

void Server::run_sync()
{
  ::openlog("data_shield", LOG_PID | LOG_CONS, LOG_USER);
  std::string addr_uri = this->hostInfo.to_str();
  try
  {
    ::syslog(LOG_INFO, "Attempting to start synchronous server on %s", addr_uri.c_str());
    auto common_helper = std::make_shared<CommonHelper>();
    auto block_helper = std::make_shared<ComponentBlockHelper>();

    auto common_service = std::make_shared<_sync::CommonService>(common_helper);
    auto comp_service = std::make_shared<_sync::ComponentBlockService>(block_helper);

    grpc::ServerBuilder builder;
    int selected_port = 0;
    // selected_port is updated with the actual port in use only after BuildAndStart() is called
    builder.AddListeningPort(addr_uri, this->creds, &selected_port);

    ::syslog(LOG_INFO, "Registering common service");
    builder.RegisterService(common_service.get());
    ::syslog(LOG_INFO, "Registering component block service");
    builder.RegisterService(comp_service.get());

    // selected_port is updated with the actual port in use
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    this->hostInfo.set_port(static_cast<uint16_t>(selected_port));
    addr_uri = this->hostInfo.to_str();
    ::syslog(LOG_INFO, "Synchronous server listening on %s", addr_uri.c_str());
    server->Wait();
    ::syslog(LOG_INFO, "Stopped server %s", addr_uri.c_str());
  }
  catch (const std::exception &ex)
  {
    ::syslog(LOG_ERR, "Synchronous server error: %s", ex.what());
    ::closelog();
    // rethrow the exception
    throw;
  }
  catch (...)
  {
    ::syslog(LOG_ERR, "Synchronous server error: Unknown exception");
    ::closelog();
    // throw a runtime exception
    throw std::runtime_error("Unknown exception");
  }
  ::closelog();
}

void Server::run_async()
{
  ::openlog("data_shield", LOG_PID | LOG_CONS, LOG_USER);
  std::string addr_uri = this->hostInfo.to_str();
  try
  {
    auto common_helper = std::make_shared<CommonHelper>();
    auto block_helper = std::make_shared<ComponentBlockHelper>();

    _async::CommonService common_async(common_helper);
    _async::ComponentBlockService comp_async(block_helper);

    grpc::ServerBuilder builder;
    int selected_port = 0;
    // selected_port is updated with the actual port in use only after BuildAndStart() is called
    builder.AddListeningPort(addr_uri, this->creds, &selected_port);

    ::syslog(LOG_INFO, "Registering common service");
    builder.RegisterService(common_async.service());
    ::syslog(LOG_INFO, "Registering component block service");
    builder.RegisterService(comp_async.service());

    std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    // selected_port is updated with the actual port in use
    this->hostInfo.set_port(static_cast<uint16_t>(selected_port));
    addr_uri = this->hostInfo.to_str();
    ::syslog(LOG_INFO, "Asynchronous server listening on %s", addr_uri.c_str());

    common_async.Start(cq.get(), std::max(1, this->initial_slots / 2));
    comp_async.Start(cq.get(), this->initial_slots);

    std::vector<std::thread> pollers;
    pollers.reserve(std::max(1, this->poller_threads));
    for (int i = 0; i < this->poller_threads; ++i)
    {
      pollers.emplace_back(
        [cq_ptr = cq.get()] {
          void* tag;
          for (bool ok; cq_ptr->Next(&tag, &ok); )
          {
            auto call = static_cast<_async::CallBase*>(tag);
            ok? call->Proceed() : call->Cancel();
          }
        });
    }

    for (auto &t : pollers)
    {
      if (t.joinable())
        t.join();
    }
    ::syslog(LOG_INFO, "Stopped server %s", addr_uri.c_str());
  }
  catch (const std::exception &ex)
  {
    ::syslog(LOG_ERR, "Asynchronous server error: %s", ex.what());
    ::closelog();
    // rethrow the exception
    throw;
  }
  catch (...)
  {
    ::syslog(LOG_ERR, "Asynchronous server error: Unknown exception");
    ::closelog();
    // throw a runtime exception
    throw std::runtime_error("Unknown exception");
  }
  ::closelog();
}
