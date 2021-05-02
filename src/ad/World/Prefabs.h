#pragma once

#include "ad/World/Entity.h"
#include "hive/ResourceManager.h"
#include "nucleus/Containers/hash_map.h"
#include "nucleus/function.h"

class Prefabs {
public:
  explicit Prefabs(hi::ResourceManager* resource_manager) : resource_manager_{resource_manager} {}

  Entity* get(EntityType entity_type) {
    auto result = prefabs_.find(entity_type);
    if (!result.was_found()) {
      return nullptr;
    }

    return &result.value();
  }

  void set(EntityType entity_type, nu::Function<void(hi::ResourceManager*, Entity*)>&& func) {
    auto result = prefabs_.insert(entity_type, {});
    Entity* storage = &result.value();

    storage->type = entity_type;

    func(resource_manager_, storage);
  }

private:
  hi::ResourceManager* resource_manager_;
  nu::HashMap<EntityType, Entity> prefabs_;
};
