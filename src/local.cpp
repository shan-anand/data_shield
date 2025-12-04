#include "local.h"
#include "Uptime.h"
#include <iomanip>

using namespace std;

int local::run_client(int argc, char **argv)
{
  if (argc < 3)
  {
    print_usage(argv[0]);
    return 1;
  }

  std::string server_address = "0.0.0.0:50051";
  int argi = 2;
  if (argc >= 4)
  {
    std::string maybe = argv[2];
    if (maybe.find(':') != std::string::npos || maybe.find("localhost") != std::string::npos || maybe.find("127.") != std::string::npos)
    {
      server_address = maybe;
      argi = 3;
    }
  }
  if (argi >= argc)
  {
    print_usage(argv[0]);
    return 1;
  }

  std::string cmd = argv[argi++];
  //auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
  Client client(server_address);

  if (cmd == "list")
  {
    common::ListApisOutput out;
    if (!client.common.ListApis("", &out))
    {
      std::cerr << "RPC error\n";
      return 1;
    }
    std::cout << "APIs:\n";
    for (const auto &a : out.apis())
      std::cout << "  " << a << "\n";
    return 0;
  }
  else if (cmd == "sysinfo")
  {
    common::SystemInfo out;
    if (!client.common.GetSystemInfo(&out))
    {
      std::cerr << "RPC error\n";
      return 1;
    }
    local::print(out);
    return 0;
  }
  else if (cmd == "add-array-powermax")
  {
    if (argi + 4 >= argc)
    {
      std::cerr << "requires 5 args\n";
      return 1;
    }
    component::block::RegisterPowerMaxInput in;
    in.set_name(argv[argi++]);
    in.set_symmetrix_id(argv[argi++]);
    in.set_dns_name(argv[argi++]);
    auto *cred = in.mutable_mgmt_credential();
    cred->set_user_name(argv[argi++]);
    cred->set_password(argv[argi++]);
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
    if (argi + 4 >= argc)
    {
      std::cerr << "requires 5 args\n";
      return 1;
    }
    component::block::RegisterPowerStoreInput in;
    in.set_name(argv[argi++]);
    in.set_appliance_id(argv[argi++]);
    in.set_dns_name(argv[argi++]);
    auto *cred = in.mutable_mgmt_credential();
    cred->set_user_name(argv[argi++]);
    cred->set_password(argv[argi++]);
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
  else if (cmd == "ping")
  {
    std::string msg = (argi < argc) ? argv[argi] : "hello";
    common::SystemInfo out;
    if (!client.common.GetSystemInfo(&out))
    {
      std::cerr << "RPC error\n";
      return 1;
    }
    local::print(out);
    return 0;
  }
  else if (cmd == "count")
  {
    component::block::ListArraysInput in;
    component::block::ListArraysOutput out;
    if (!client.component_block.ListArrays(in, &out))
    {
      std::cerr << "RPC error\n";
      return 1;
    }
    std::cout << "Array count: " << out.arrays_size() << "\n";
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
  cout << "Uptime......: " << uptime.to_epoch() << " (" << uptime.to_string() << ")" << endl;
  if (systemInfo.has_ram())
    local::print(systemInfo.ram(), "RAM");
  if (systemInfo.has_swap())
    local::print(systemInfo.swap(), "Swap");
}
