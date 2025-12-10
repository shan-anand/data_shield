#include "service/common_helper.h"
#include <sys/sysinfo.h>
#include <thread>
#include <chrono>

using namespace data_shield;

CommonHelper::CommonHelper() : start_time_(std::chrono::steady_clock::now())
{
}

common::SystemInfo CommonHelper::GetSystemInfo(const common::SystemInfoInput& in) const
{
  common::SystemInfo out;

  auto get_time = []() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);
    return std::tuple<std::int64_t, std::int32_t>{seconds.count(), static_cast<std::int32_t>(nanos.count())};
  };

  // If sleep time is requested, sleep for that duration
  if ( in.has_sleep_time() )
  {
    int sleep_secs = in.sleep_time();
    if ( sleep_secs >= 0 )
      sleep_secs = std::min(sleep_secs, 30); // cap to 30 seconds
    else if ( sleep_secs < 0 )
    {
      auto [seconds, nanos] = get_time();
      sleep_secs = std::min((nanos % 5)+1, 5); // sleep between 1 to 5 seconds
    }
    std::this_thread::sleep_for(std::chrono::seconds(sleep_secs));
    out.set_slept_for(sleep_secs);
  }

  // Start populating the SystemInfo response
  auto [seconds, nanos] = get_time();
  google::protobuf::Timestamp *timestamp = out.mutable_timestamp();
  timestamp->set_seconds(seconds);
  timestamp->set_nanos(nanos);
  struct sysinfo si;
  if (0 == sysinfo(&si))
  {
    out.set_uptime(si.uptime);
    common::Space *ram = out.mutable_ram();
    ram->set_total(si.totalram);
    ram->set_used(si.totalram - si.freeram);
    ram->set_free(si.freeram);
    common::Space *swap = out.mutable_swap();
    swap->set_total(si.totalswap);
    swap->set_used(si.totalswap - si.freeswap);
    swap->set_free(si.freeram);
  }
  return out;
}

common::ListApisOutput CommonHelper::ListApis(const common::ListApisInput &in) const
{
  common::ListApisOutput out;
  std::vector<std::string> all = {"common.Api", "component.block.Api"};
  for (const auto &a : all)
  {
    if (!in.search().empty())
    {
      if (a.find(in.search()) == std::string::npos)
        continue;
    }
    out.add_apis(a);
  }
  return out;
}
