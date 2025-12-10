#include "cli/args.h"
#include "cli/uptime.h"
#include "cli/client.h"
#include "cli/server.h"
#include <iomanip>

using namespace std;
using namespace data_shield::cli;

namespace local
{
  void print_usage(const char* prog);
  Action get_action(std::string_view actionStr);
  int run_client(int argc, char **argv);
  int run_server_sync(int argc, char **argv);
  int run_server_async(int argc, char **argv);

  void print(const common::Space &_space, std::string_view _prefix);
  void print(const google::protobuf::Timestamp &_timestamp);
  void print(const common::SystemInfo &systemInfo);
  component::block::JobModel parse_job_model(const std::string& jobModelStr);
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    local::print_usage(argv[0]);
    return 1;
  }

  try
  {
    const Action action = local::get_action(argv[1]);
    switch ( action )
    {
    case Action::Client:
      return local::run_client(argc, argv);
    case Action::ServerSync:
      return local::run_server_sync(argc, argv);
    case Action::ServerAsync:
      return local::run_server_async(argc, argv);
    }
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return -1;
  }
  return 0;
}

void local::print_usage(const char *prog)
{
  std::cerr << "Usage:\n";
  std::cerr << "  " << prog << " <ACTION> ACTION-SPECIFIC-PARAMS...\n";
  std::cerr << "  " << prog << " server|server-sync [--background] [--addr=ip_addr[:port]] [--port=<PORT>]\n";
  std::cerr << "  " << prog << " server-async [--background] [--addr=ip_addr[:port]] [--port=<PORT>] [--threads=NUM_THREADS] [--slots=INITIAL_SLOTS_PER_RPC]\n";
  std::cerr << "            Defaults: --addr=0.0.0.0:50051 NUM_THREADS=4 INITIAL_SLOTS_PER_RPC=4\n";
  std::cerr << "  " << prog << " client [--addr=ip_addr[:port]] [--port=<PORT>] <command> [args...]\n";
  std::cerr << "Client commands:\n";
  std::cerr << "  list                       (list apis)\n";
  std::cerr << "  sysinfo                    (get system info)\n";
  std::cerr << "  add-array-powermax <name> <symmetrix_id> <dns> <user> <pass>\n";
  std::cerr << "  add-array-powerstore <name> <appliance_id> <dns> <user> <pass>\n";
  std::cerr << "  list-arrays\n";
  std::cerr << "  ping <msg>\n";
  std::cerr << "  count\n";
}


Action local::get_action(std::string_view actionStr)
{
  if ( actionStr.substr(0, 2) == "--" )
    actionStr = actionStr.substr(2);
  if ( actionStr == "client" )
    return Action::Client;
  else if ( actionStr == "server" || actionStr == "server-sync" )
    return Action::ServerSync;
  else if ( actionStr == "server-async" )
    return Action::ServerAsync;

  throw std::invalid_argument("Invalid action " + std::string(actionStr)
   + ". Must be one of: client, server|server-sync, server-async");
}

int local::run_server_sync(int argc, char **argv)
{
  Args args = Args::parse(argc, argv, 2);
  Server server(args.get_host());
  server.runMode = Server::RunMode::Sync;
  for ( const auto& arg : args )
  {
    if ( arg.key == "--background" )
      server.background = true;
  }
  // Run the server
  server.run();
  if ( server.background )
    cout << "Synchronous Server running in background on " << server.hostInfo.to_str() << endl;
  return 0;
}

int local::run_server_async(int argc, char **argv)
{
  Args args = Args::parse(argc, argv, 2);
  Server server(args.get_host());
  server.runMode = Server::RunMode::Async;
  for ( const auto& arg : args )
  {
    if ( arg.key == "--background" )
      server.background = true;
    else if ( arg.key == "--threads" )
    {
      if ( !arg.value.has_value() )
        throw std::invalid_argument(arg.key + " requires a value");
      int pollers = std::atoi(arg.value.value().c_str());
      if ( pollers > 0 )
        server.poller_threads = pollers;
      else
      {
        cout << "Invalid poller_threads value provided: " << arg.value.value()
             << " (using default: " << server.poller_threads << ")" << endl;
      }
    }
    else if ( arg.key == "--slots" )
    {
      if ( !arg.value.has_value() )
        throw std::invalid_argument(arg.key + " requires a value");
      int slots = std::atoi(arg.value.value().c_str());
      if ( slots > 0 )
        server.initial_slots = slots;
      else
      {
        cout << "Invalid initial_slots value provided: " << arg.value.value()
             << " (using default: " << server.initial_slots << ")" << endl;
      }
    }
  }
  // Run the server
  server.run();
  if ( server.background )
    cout << "Asynchronous Server running in background on " << server.hostInfo.to_str() << endl;
  return 0;
}

int local::run_client(int argc, char **argv)
{
  Args hostArgs, clientArgs;
  std::string cmd;
  bool cmdFound = false;
  for ( int i = 2; i < argc; i++ )
  {
    std::string argStr = argv[i];
    if ( !cmdFound && argStr[0] != '-' )
    {
      cmd = argStr;
      cmdFound = true;
    }
    else
    {
      (cmdFound? clientArgs : hostArgs).push_back(Arg::parse(argv[i]));
    }
  }

  if ( !cmdFound )
  {
    print_usage(argv[0]);
    return 1;
  }

  HostInfo hostInfo = hostArgs.get_host();
  cout << "Connecting to server at " << hostInfo.to_str() << endl;
  //auto channel = grpc::CreateChannel(hostInfo.to_str(), grpc::InsecureChannelCredentials());
  Client client(hostInfo.to_str());

  if (cmd == "sysinfo")
  {
    common::SystemInfoInput in;
    for ( const auto& arg : clientArgs )
    {
      if ( arg.key == "--sleep-time" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_sleep_time(std::atoi(arg.value.value().c_str()));
      }
    }

    common::SystemInfo out;
    if (!client.common.GetSystemInfo(in, &out))
      throw std::runtime_error("RPC error");
    local::print(out);
    return 0;
  }
  else if (cmd == "list")
  {
    common::ListApisOutput out;
    if (!client.common.ListApis("", &out))
      throw std::runtime_error("RPC error");
    std::cout << "APIs:\n";
    for (const auto &a : out.apis())
      std::cout << "  " << a << "\n";
    return 0;
  }
  else if (cmd == "add-array-powermax")
  {
    component::block::RegisterPowerMaxInput in;
    for ( const auto& arg : clientArgs )
    {
      if ( arg.key == "--name" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_name(arg.value.value());
      }
      else if ( arg.key == "--symmetrix-id" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_symmetrix_id(arg.value.value());
      }
      else if ( arg.key == "--dns-name" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_dns_name(arg.value.value());
      }
      else if ( arg.key == "--username" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.mutable_mgmt_credential()->set_user_name(arg.value.value());
      }
      else if ( arg.key == "--password" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.mutable_mgmt_credential()->set_password(arg.value.value());
      }
      else if ( arg.key == "--max-device-limit" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_max_device_use_limit(std::atoi(arg.value.value().c_str()));
      }
    }
    component::block::RegisterArrayOutput out;
    if (!client.component_block.RegisterPowerMax(in, &out) || !out.success())
    {
      std::cerr << "Register failed\n";
      return 1;
    }
    std::cout << "Registered array: " << out.array().name() << "\n";
    return 0;
  }
  else if (cmd == "add-array-powerstore")
  {
    component::block::RegisterPowerStoreInput in;
    for ( const auto& arg : clientArgs )
    {
      if ( arg.key == "--name" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_name(arg.value.value());
      }
      else if ( arg.key == "--appliance-id" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_appliance_id(arg.value.value());
      }
      else if ( arg.key == "--dns-name" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_dns_name(arg.value.value());
      }
      else if ( arg.key == "--username" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.mutable_mgmt_credential()->set_user_name(arg.value.value());
      }
      else if ( arg.key == "--password" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.mutable_mgmt_credential()->set_password(arg.value.value());
      }
      else if ( arg.key == "--max-device-limit" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_max_device_use_limit(std::atoi(arg.value.value().c_str()));
      }
      else if ( arg.key == "--job-model" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        component::block::JobModel jobModel = local::parse_job_model(arg.value.value());
        if ( !component::block::JobModel_IsValid(jobModel) )
        {
          std::cerr << "Invalid job model: " << arg.value.value() << "\n";
          return 1;
        }
        in.set_job_model(jobModel);
      } 
    }
    component::block::RegisterArrayOutput out;
    if (!client.component_block.RegisterPowerStore(in, &out) || !out.success())
    {
      std::cerr << "Register failed\n";
      return 1;
    }
    std::cout << "Registered array: " << out.array().name() << "\n";
    return 0;
  }
  else if (cmd == "list-arrays")
  {
    component::block::ListArraysInput in;
    for ( const auto& arg : clientArgs )
    {
      if ( arg.key == "--name" )
      {
        if ( !arg.value.has_value() )
          throw std::invalid_argument(arg.key + " requires a value");
        in.set_name(arg.value.value());
      }
    }
    component::block::ListArraysOutput out;
    if (!client.component_block.ListArrays(in, &out))
    {
      std::cerr << "RPC error\n";
      return 1;
    }
    std::cout << "Arrays:\n";
    for (const auto &a : out.arrays())
    {
      std::cout << "  name=" << a.name() << " dns=" << a.dns_name() << "\n";
    }
    return 0;
  }

  std::cerr << "Unknown client command\n";
  print_usage(argv[0]);
  return 1;
}

void local::print(const common::Space &_space, std::string_view _prefix)
{
  cout << _prefix << " total...: " << _space.total() << endl;
  cout << _prefix << " used....: " << _space.used() << endl;
  cout << _prefix << " free....: " << _space.free() << endl;
}

void local::print(const google::protobuf::Timestamp &_timestamp)
{
  // auto dt = std::chrono::system_clock::from_time_t(_timestamp.seconds());
  // Uptime uptime(_timestamp.seconds());
  cout << "Timestamp...: " << _timestamp.seconds()
       << "." << std::setfill('0') << std::setw(9) << _timestamp.nanos() << endl;
}

void local::print(const common::SystemInfo &systemInfo)
{
  local::print(systemInfo.timestamp());
  Uptime uptime(systemInfo.uptime());
  cout << "Uptime......: " << uptime.to_epoch() << " (" << uptime.to_str() << ")" << endl;
  if (systemInfo.has_ram())
    local::print(systemInfo.ram(), "RAM");
  if (systemInfo.has_swap())
    local::print(systemInfo.swap(), "Swap");
  if ( systemInfo.has_slept_for() )
    cout << "Sleep time...: " << systemInfo.slept_for() << " seconds" << endl;
}

  component::block::JobModel local::parse_job_model(const std::string& jobModelStr)
  {
    if ( jobModelStr == "push" )
      return component::block::JobModel::JOB_MODEL_PUSH;
    else if ( jobModelStr == "pull" )
      return component::block::JobModel::JOB_MODEL_PULL;
    else if ( jobModelStr == "both" )
      return component::block::JobModel::JOB_MODEL_BOTH;
    else
      return component::block::JobModel::JOB_MODEL_UNKNOWN;
  }
