#include "local.h"

using namespace std;

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
