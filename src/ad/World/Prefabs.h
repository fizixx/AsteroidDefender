#pragma once

#include "ad/World/Entity.h"
#include "hive/ResourceManager.h"
#include "nucleus/Containers/hash_map.h"
#include "nucleus/function.h"

class Prefabs {
public:
  explicit Prefabs(hi::ResourceManager* resource_manager) : resource_manager_{resource_manager} {}

  Entity* get(EntityType entity_type) {
    auto it = prefabs_.find((MemSize)entity_type);
    if (it == prefabs_.end()) {
      return nullptr;
    }

    return &it->value;
  }

  void set(EntityType entity_type, nu::Function<void(hi::ResourceManager*, Entity*)>&& func) {
    prefabs_.set((MemSize)entity_type, {});
    auto it = prefabs_.find((MemSize)entity_type);
    Entity* storage = &it->value;

    storage->type = entity_type;

    func(resource_manager_, storage);
  }

private:
  hi::ResourceManager* resource_manager_;
  nu::HashMap<MemSize, Entity> prefabs_;
};
