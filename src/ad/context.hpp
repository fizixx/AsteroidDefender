#pragma once

#include "World/prefabs.h"
#include "World/world.h"

namespace ad {

// Shared context between all engine layers.
class Context : public nu::RefCounted<Context> {
public:
  World& world() {
    return world_;
  }

  Prefabs& prefabs() {
    DCHECK(prefabs_);
    return *prefabs_;
  }

  void initialize_prefabs(le::ResourceManager* resource_manager) {
    prefabs_ = nu::make_scoped_ptr<Prefabs>(resource_manager);
  }

private:
  World world_;
  nu::ScopedPtr<Prefabs> prefabs_;
};

}  // namespace ad
