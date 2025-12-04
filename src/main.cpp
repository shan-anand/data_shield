#include "local.h"

using namespace std;

void local::print_usage(const char *prog)
{
  std::cerr << "Usage:\n";
  std::cerr << "  " << prog << " --server-sync [address]\n";
  std::cerr << "  " << prog << " --server-async [address] [poller_threads] [initial_slots_per_rpc]\n";
  std::cerr << "  " << prog << " --server (synonym for --server-sync) [address]\n";
  std::cerr << "  " << prog << " --client [address] <command> [args...]\n";
  std::cerr << "Defaults: address=0.0.0.0:50051 poller_threads=4 initial_slots_per_rpc=4\n";
  std::cerr << "Client commands:\n";
  std::cerr << "  list                       (list apis)\n";
  std::cerr << "  sysinfo                    (get system info)\n";
  std::cerr << "  add-array-powermax <name> <symmetrix_id> <dns> <user> <pass>\n";
  std::cerr << "  add-array-powerstore <name> <appliance_id> <dns> <user> <pass>\n";
  std::cerr << "  list-arrays\n";
  std::cerr << "  ping <msg>\n";
  std::cerr << "  count\n";
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    local::print_usage(argv[0]);
    return 1;
  }

  std::string mode = argv[1];
  if (mode == "--client")
    return local::run_client(argc, argv);

  if (mode != "--server-sync" && mode != "--server-async" && mode != "--server")
  {
    local::print_usage(argv[0]);
    return 1;
  }

  Server server;
  if (argc >= 3)
    server.address = argv[2];
  if (mode == "--server-sync" || mode == "--server")
  {
    server.run_sync();
  }
  else
  {
    if (argc >= 4) {
      int pollers = std::atoi(argv[3]);
      if ( pollers > 0 )
        server.poller_threads = pollers;
      else
      {
        cout << "Invalid poller_threads value provided: " << argv[3]
             << " (using default: " << server.poller_threads << ")" << endl;
      }
    }
    if (argc >= 5) {
      int slots = std::atoi(argv[4]);
      if ( slots > 0 )
        server.initial_slots = slots;
      else
      {
        cout << "Invalid initial_slots value provided: " << argv[4]
             << " (using default: " << server.initial_slots << ")" << endl;
      }
    }
    server.run_async();
  }

  return 0;
}