#pragma once

#include "ad/world/construction_controller.h"
#include "ad/world/prefabs.h"
#include "ad/world/world.h"

namespace ad {

// Shared context between all engine layers.
class Context : public nu::RefCounted<Context> {
public:
  explicit Context(le::ResourceManager* resource_manager)
    : resource_manager_{resource_manager},
      prefabs_{resource_manager},
      construction_controller_{&world_, &prefabs_} {}

  World& world() {
    return world_;
  }

  Prefabs& prefabs() {
    return prefabs_;
  }

  ConstructionController& construction_controller() {
    return construction_controller_;
  }

private:
  le::ResourceManager* resource_manager_;
  World world_;
  Prefabs prefabs_;

  ConstructionController construction_controller_;
};

}  // namespace ad
