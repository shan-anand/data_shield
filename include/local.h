#pragma once

#include <grpcpp/grpcpp.h>
#include "service/server.h"
#include "client/client.h"
#include "util/host_info.h"

#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <optional>

using namespace data_shield;

enum class Action : uint8_t {
  Client = 1,
  ServerSync = 2,
  ServerAsync = 3
};

struct Arg
{
  std::string key;
  std::optional<std::string> value;

  Arg() = default;
  Arg(const std::string& _key) : key(_key) {}
  Arg(const std::string& _key, const std::string& _value) : key(_key), value(_value) {}
  Arg(const std::string& _key, std::optional<std::string> _value) : key(_key), value(_value) {}

  static Arg parse(const std::string& argStr)
  {
    if ( auto eqPos = argStr.find('='); eqPos != std::string::npos )
      return Arg(argStr.substr(0, eqPos), argStr.substr(eqPos + 1));
    return Arg(argStr);
  }
};

struct Args : public std::vector<Arg>
{
  std::optional<Arg> get(std::string_view key)
  {
    auto it = std::find_if(this->begin(), this->end(), [key](const Arg& arg) { return arg.key == key; });
    return ( it != this->end() )? *it : std::optional<Arg>{};
  }

  static Args parse(int argc, char**argv, int startIndex)
  {
    Args args;
    for ( int i = startIndex; i < argc; i++ )
      args.push_back(Arg::parse(argv[i]));
    return args;
  }

  util::HostInfo get_host() const
  {
    util::HostInfo hostInfo;
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
};


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
