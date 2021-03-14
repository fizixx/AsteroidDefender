#ifndef WORLD_H_
#define WORLD_H_

#include "ad/World/Entity.h"
#include "ad/World/Resources.h"
#include "ad/World/Systems/MovementSystem.h"
#include "ad/World/Systems/ResourceSystem.h"
#include "legion/World/Camera.h"
#include "nucleus/Containers/DynamicArray.h"
#include "nucleus/Macros.h"

namespace ca {
class Renderer;
}

namespace hi {
class ResourceManager;
}

namespace re {
struct Model;
}

class World {
public:
  NU_DELETE_COPY_AND_MOVE(World);

  World() = default;
  ~World() = default;

  auto resources() -> Resources* {
    return &m_resources;
  }

  auto clear() -> void;
  auto addEntityFromPrefab(Entity* prefab, const fl::Vec2& position) -> EntityId;

  void setCursorPosition(const fl::Vec2& position);
  auto getEntityUnderCursor() const -> EntityId;

  void tick(F32 delta);
  void render(ca::Renderer* renderer, le::Camera* camera);

private:
  fl::Vec2 m_cursorPosition{fl::Vec2::zero};

  EntityList m_entities;

  Resources m_resources;

  MovementSystem m_movementSystem;
  ResourceSystem m_resourceSystem{&m_resources};
};

#endif  // WORLD_H_
