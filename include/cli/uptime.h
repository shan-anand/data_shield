#pragma once

#include <string>
#include <cstdint>

namespace data_shield::cli {

struct Uptime
{
  uint64_t seconds : 6;
  uint64_t minutes : 6;
  uint64_t hours   : 5;
  uint64_t days    : 47;

  Uptime(uint64_t _epoch = 0);
  Uptime& set(uint64_t _epoch);
  uint64_t to_epoch() const;
  std::string to_str() const;
};

} // namespace data_shield::cli