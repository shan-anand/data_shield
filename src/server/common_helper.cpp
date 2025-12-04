#include "server/common_helper.h"
#include <sys/sysinfo.h>
#include <thread>
#include <chrono>

CommonHelper::CommonHelper() : start_time_(std::chrono::steady_clock::now())
{
}

common::SystemInfo CommonHelper::GetSystemInfo() const
{
  common::SystemInfo out;
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
  auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);
  google::protobuf::Timestamp *timestamp = out.mutable_timestamp();
  timestamp->set_seconds(seconds.count());
  timestamp->set_nanos(nanos.count());
  std::this_thread::sleep_for(std::chrono::seconds(nanos.count() % 5));
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
