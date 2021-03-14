#pragma once

#include "ad/World/Entity.h"
#include "hive/ResourceManager.h"

#include <unordered_map>

class Prefabs {
public:
  explicit Prefabs(hi::ResourceManager* resource_manager) : resource_manager_{resource_manager} {}

  auto get(EntityType entity_type) -> Entity* {
    auto it = prefabs_.find(entity_type);
    if (it == prefabs_.end()) {
      return nullptr;
    }

    return &it->second;
  }

  template <typename Func>
  auto set(EntityType entity_type, Func func) -> void {
    prefabs_[entity_type] = {};

    Entity* storage = &prefabs_[entity_type];
    storage->type = entity_type;

    func(resource_manager_, storage);
  }

private:
  hi::ResourceManager* resource_manager_;
  std::unordered_map<EntityType, Entity> prefabs_;
};
