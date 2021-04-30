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

    return &it->second;
  }

  void set(EntityType entity_type, nu::Function<void(hi::ResourceManager*, Entity*)>&& func) {
    auto [it, inserted] = prefabs_.insert({(MemSize)entity_type, {}});
    Entity* storage = &it->second;

    storage->type = entity_type;

    func(resource_manager_, storage);
  }

private:
  hi::ResourceManager* resource_manager_;
  std::unordered_map<MemSize, Entity> prefabs_;
};
