#pragma once

#include "generated/common.pb.h"
#include <chrono>

class CommonHelper {
 public:
  CommonHelper();
  ~CommonHelper() = default;

  // Returns a populated common::SystemInfo (timestamp set to now, uptime in seconds)
  common::SystemInfo GetSystemInfo(const common::SystemInfoInput& in) const;
  // Returns a populated common::ListApisOutput
  common::ListApisOutput ListApis(const common::ListApisInput& in) const;

 private:
  std::chrono::steady_clock::time_point start_time_;
};
