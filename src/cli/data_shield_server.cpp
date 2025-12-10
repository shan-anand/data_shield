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
  int run_server(const Server::RunMode runMode, Args& args);
}

int main(int argc, char **argv)
{
  try
  {
    if (argc < 2)
    {
      local::print_usage(argv[0]);
      return 1;
    }
   
    const Server::RunMode runMode = Server::getRunMode(argv[1]);
    Args args = Args::parse(argc, argv, 2);
    return local::run_server(runMode, args);
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return -1;
  }
  return 0;
}

/*static*/
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

namespace local
{
  int run_server_sync(Server& server, Args& args);
  int run_server_async(Server& server, Args& args);
  // Arguments parsers
  bool get_background(Args& args, const bool defaultValue);
  int get_threads(Args& args, const int defaultValue);
  int get_slots(Args& args, const int defaultValue);
  int get_value(Args& args, const std::string& key, const int defaultValue, const int minValue, const int maxValue);
}

int local::run_server(const Server::RunMode runMode, Args& args)
{
  Server server(args.get_host());
  server.runMode = runMode;
  server.background = local::get_background(args, server.background);
  // Run the server
  switch ( runMode )
  {
  case Server::RunMode::Sync:
    return local::run_server_sync(server, args);
  case Server::RunMode::Async:
    return local::run_server_async(server, args);
  }
  if ( server.background )
    cout << "Server running in background on " << server.hostInfo.to_str() << endl;
  return 0;
}

int local::run_server_sync(Server& server, Args& args)
{
  // Run the server
  server.run();
  if ( server.background )
    cout << "Synchronous Server running in background on " << server.hostInfo.to_str() << endl;
  return 0;
}

int local::run_server_async(Server& server, Args& args)
{
  // Get async specific params
  server.poller_threads = local::get_threads(args, server.poller_threads);
  server.initial_slots = local::get_slots(args, server.initial_slots);
  // Run the server
  server.run();
  if ( server.background )
    cout << "Asynchronous Server running in background on " << server.hostInfo.to_str() << endl;
  return 0;
}

bool local::get_background(Args& args, const bool defaultValue)
{
  auto it = std::find_if(args.begin(), args.end(),
              [](const Arg& arg) { return arg.key == "--background"; });
  if ( it == args.end() )
    return defaultValue;
  args.erase(it);
  return true;
}

int local::get_threads(Args& args, const int defaultValue)
{
  return local::get_value(args, "--threads", defaultValue, 1, 50);
}

int local::get_slots(Args& args, const int defaultValue)
{
  return local::get_value(args, "--slots", defaultValue, 1, 100);
}

int local::get_value(Args& args, const std::string& key, const int defaultValue, const int minValue, const int maxValue)
{
  auto it = std::find_if(args.begin(), args.end(),
              [&key](const Arg& arg) { return arg.key == key; });
  if ( it == args.end() )
    return defaultValue;

  if ( !it->value.has_value() )
    throw std::invalid_argument(it->key + " requires a value");
  int value = std::atoi(it->value.value().c_str());
  if ( value < minValue || value > maxValue )
  {
    cout << it->key << " (" << value << ") not in range "
          << minValue << " - " << maxValue
          << " (using default: " << defaultValue << ")" << endl;
    value = defaultValue;
  }
  args.erase(it);
  return value;
}

