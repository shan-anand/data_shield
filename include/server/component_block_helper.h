#pragma once

#include "generated/component_block.pb.h"

#include <shared_mutex>
#include <vector>

namespace data_shield {

class ComponentBlockHelper
{
 public:
  ComponentBlockHelper();
  ~ComponentBlockHelper() = default;

  // Register arrays (returns the created ArrayInfo)
  component::block::ArrayInfo RegisterPowerMax(const component::block::RegisterPowerMaxInput& in);
  component::block::ArrayInfo RegisterPowerStore(const component::block::RegisterPowerStoreInput& in);

  // List arrays with optional filters
  std::vector<component::block::ArrayInfo> ListArrays(const component::block::ListArraysInput& in) const;

 private:
  mutable std::shared_mutex lock_;
  std::vector<component::block::ArrayInfo> arrays_;
  uint64_t next_id_; // used to generate unique names if needed
};

} // namespace data_shield
