#include "server/component_block_helper.h"

#include <algorithm>
#include <string>

using component::block::ArrayInfo;
using component::block::RegisterPowerMaxInput;
using component::block::RegisterPowerStoreInput;
using component::block::ListArraysInput;
using component::block::ArrayType;
using component::block::PowerMaxSpecific;
using component::block::PowerStoreSpecific;

ComponentBlockHelper::ComponentBlockHelper() : next_id_(1) {}

ArrayInfo ComponentBlockHelper::RegisterPowerMax(const RegisterPowerMaxInput& in) {
  ArrayInfo info;
  info.set_type(ArrayType::ARRAY_TYPE_POWER_MAX);
  info.set_name(in.name());
  info.set_dns_name(in.dns_name());
  info.mutable_mgmt_credential()->CopyFrom(in.mgmt_credential());
  info.set_max_device_use_limit(in.max_device_use_limit());
  info.set_job_model(component::block::JOB_MODEL_UNKNOWN);

  PowerMaxSpecific* pmax = info.mutable_power_max();
  pmax->set_symmetrix_id(in.symmetrix_id());

  {
    std::unique_lock<std::shared_mutex> wlock(lock_);
    bool exists = std::any_of(arrays_.begin(), arrays_.end(),
                              [&info](const ArrayInfo& a) { return a.name() == info.name(); });
    if (exists) {
      info.set_name(info.name() + "-" + std::to_string(next_id_++));
    }
    arrays_.push_back(info);
  }

  return info;
}

ArrayInfo ComponentBlockHelper::RegisterPowerStore(const RegisterPowerStoreInput& in) {
  ArrayInfo info;
  info.set_type(ArrayType::ARRAY_TYPE_POWER_STORE);
  info.set_name(in.name());
  info.set_dns_name(in.dns_name());
  info.mutable_mgmt_credential()->CopyFrom(in.mgmt_credential());
  info.set_max_device_use_limit(in.max_device_use_limit());
  info.set_job_model(in.job_model());

  PowerStoreSpecific* ps = info.mutable_power_store();
  ps->set_appliance_id(in.appliance_id());

  {
    std::unique_lock<std::shared_mutex> wlock(lock_);
    bool exists = std::any_of(arrays_.begin(), arrays_.end(),
                              [&info](const ArrayInfo& a) { return a.name() == info.name(); });
    if (exists) {
      info.set_name(info.name() + "-" + std::to_string(next_id_++));
    }
    arrays_.push_back(info);
  }

  return info;
}

std::vector<ArrayInfo> ComponentBlockHelper::ListArrays(const ListArraysInput& in) const {
  std::vector<ArrayInfo> out;
  std::shared_lock<std::shared_mutex> rlock(lock_);
  for (const auto& a : arrays_) {
    if (!in.name().empty() && a.name() != in.name()) continue;
    if (!in.array_id().empty()) {
      // array_id not explicitly stored; match against name as a simple policy
      if (a.name() != in.array_id()) continue;
    }
    if (in.type() != ArrayType::ARRAY_TYPE_UNKNOWN && a.type() != in.type()) continue;
    out.push_back(a);
  }
  return out;
}