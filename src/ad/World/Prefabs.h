#ifndef AD_WORLD_PREFABS_H_
#define AD_WORLD_PREFABS_H_

#include "ad/World/Entity.h"
#include "hive/ResourceManager.h"

#include <unordered_map>

class Prefabs {
public:
  explicit Prefabs(hi::ResourceManager* resourceManager) : m_resourceManager{resourceManager} {}

  auto get(EntityType entityType) -> Entity* {
    auto it = m_prefabs.find(entityType);
    if (it == m_prefabs.end()) {
      return nullptr;
    }

    return &it->second;
  }

  template <typename Func>
  auto set(EntityType entityType, Func func) -> void {
    m_prefabs[entityType] = {};

    Entity* storage = &m_prefabs[entityType];
    storage->type = entityType;

    func(m_resourceManager, storage);
  }

private:
  hi::ResourceManager* m_resourceManager;
  std::unordered_map<EntityType, Entity> m_prefabs;
};

#endif  // AD_WORLD_PREFABS_H_
