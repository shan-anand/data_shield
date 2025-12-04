#include "local.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;

void local::print_usage(const char* prog) {
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

int main(int argc, char** argv) {
  if (argc < 2) { local::print_usage(argv[0]); return 1; }

  std::string mode = argv[1];
  if (mode == "--client") return local::run_client(argc, argv);

  if (mode != "--server-sync" && mode != "--server-async" && mode != "--server") {
    local::print_usage(argv[0]); return 1;
  }

  std::string server_address = "0.0.0.0:50051";
  if (mode == "--server-sync" || mode == "--server") {
    if (argc >= 3) server_address = argv[2];
    local::run_server_sync(server_address);
  } else {
    int pollers = 4, slots = 4;
    if (argc >= 3) server_address = argv[2];
    if (argc >= 4) pollers = std::atoi(argv[3]);
    if (argc >= 5) slots = std::atoi(argv[4]);
    if (pollers <= 0) pollers = 1;
    if (slots <= 0) slots = 1;
    local::run_server_async(server_address, pollers, slots);
  }

  return 0;
}