#include "cli/args.h"
#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace data_shield::cli;

Arg Arg::parse(const std::string& argStr)
{
  if ( auto eqPos = argStr.find('='); eqPos != std::string::npos )
    return Arg(argStr.substr(0, eqPos), argStr.substr(eqPos + 1));
  return Arg(argStr);
}

std::optional<Arg> Args::get(std::string_view key)
{
  auto it = std::find_if(this->begin(), this->end(), [key](const Arg& arg) { return arg.key == key; });
  return ( it != this->end() )? *it : std::optional<Arg>{};
}

Args Args::parse(int argc, char**argv, int startIndex)
{
  Args args;
  for ( int i = startIndex; i < argc; i++ )
    args.push_back(Arg::parse(argv[i]));
  return args;
}

HostInfo Args::get_host() const
{
  HostInfo hostInfo;
  auto it = std::find_if(this->begin(), this->end(), [](const Arg& arg) { return arg.key == "--addr"; });
  if ( it != this->end() )
  {
    if ( !it->value.has_value() )
      throw std::invalid_argument("Address argument --addr requires a value");
    hostInfo.set(it->value.value());
  }
  // --port overrides port in --addr
  it = std::find_if(this->begin(), this->end(), [](const Arg& arg) { return arg.key == "--port"; });
  if ( it != this->end() )
  {
    if ( !it->value.has_value() )
      throw std::invalid_argument("Port argument --port requires a value");
    int port = std::atoi(it->value.value().c_str());
    hostInfo.set_port(static_cast<uint16_t>(port));
  }
  return hostInfo;
}
