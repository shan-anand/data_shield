#include "local.h"
#include "util/uptime.h"
#include <iomanip>

using namespace std;

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

  util::HostInfo hostInfo = hostArgs.get_host();
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
  util::Uptime uptime(systemInfo.uptime());
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