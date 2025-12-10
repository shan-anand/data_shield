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
  int run_client(Args hostArgs, const std::string& cmd, Args clientArgs);
}

int main(int argc, char **argv)
{
  int retVal = 0;
  try
  {
    if (argc < 2)
    {
      local::print_usage(argv[0]);
      return 1;
    }
    Args hostArgs, clientArgs;
    std::string cmd;
    bool cmdFound = false;
    for ( int i = 1; i < argc; i++ )
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
      local::print_usage(argv[0]);
      return 1;
    }
    retVal = local::run_client(hostArgs, cmd, clientArgs);
    if ( retVal == 1 )
      local::print_usage(argv[0]);
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return -1;
  }
  return retVal;
}

void local::print_usage(const char *prog)
{
  std::cerr << "Usage:\n";
  std::cerr << "  " << prog << " [--addr=ip_addr[:port]] [--port=<PORT>] <command> [args...]\n";
  std::cerr << "            Defaults: --addr=0.0.0.0 --port=50051\n";
  std::cerr << "Client commands:\n";
  std::cerr << "  list                       (list apis)\n";
  std::cerr << "  sysinfo                    (get system info)\n";
  std::cerr << "  add-array-powermax <name> <symmetrix_id> <dns> <user> <pass>\n";
  std::cerr << "  add-array-powerstore <name> <appliance_id> <dns> <user> <pass>\n";
  std::cerr << "  list-arrays\n";
}

// Support functions for client commands
namespace local
{
  void print(const common::Space &_space, std::string_view _prefix);
  void print(const google::protobuf::Timestamp &_timestamp);
  void print(const common::SystemInfo &systemInfo);
  component::block::JobModel parse_job_model(const std::string& jobModelStr);
}

int local::run_client(Args hostArgs, const std::string& cmd, Args clientArgs)
{
  int retVal = 0;
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
  }
  else if (cmd == "list")
  {
    common::ListApisOutput out;
    if (!client.common.ListApis("", &out))
      throw std::runtime_error("RPC error");
    std::cout << "APIs:\n";
    for (const auto &a : out.apis())
      std::cout << "  " << a << "\n";
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
      throw std::runtime_error("Register failed");
    std::cout << "Registered array: " << out.array().name() << "\n";
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
          throw std::runtime_error("Invalid job model: " + arg.value.value());
        in.set_job_model(jobModel);
      } 
    }
    component::block::RegisterArrayOutput out;
    if (!client.component_block.RegisterPowerStore(in, &out) || !out.success())
      throw std::runtime_error("Register failed");
    std::cout << "Registered array: " << out.array().name() << "\n";
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
      throw std::runtime_error("RPC error");
    std::cout << "Arrays:\n";
    for (const auto &a : out.arrays())
    {
      std::cout << "  name=" << a.name() << " dns=" << a.dns_name() << "\n";
    }
  }
  else
  {
    retVal = 1; // command not found
  }

  return retVal;
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
