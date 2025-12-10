#pragma once

#include "cli/host_info.h"

#include <vector>
#include <string>
#include <optional>
#include <string_view>

namespace data_shield::cli {

struct Arg
{
  std::string key;
  std::optional<std::string> value;

  Arg() = default;
  Arg(const std::string& _key) : key(_key) {}
  Arg(const std::string& _key, const std::string& _value) : key(_key), value(_value) {}
  Arg(const std::string& _key, std::optional<std::string> _value) : key(_key), value(_value) {}

  static Arg parse(const std::string& argStr);
};

struct Args : public std::vector<Arg>
{
  std::optional<Arg> get(std::string_view key);
  static Args parse(int argc, char**argv, int startIndex);
  HostInfo get_host() const;
};

} // namespace data_shield::cli
