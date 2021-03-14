#pragma once

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

class World {
public:
  NU_DELETE_COPY_AND_MOVE(World);

  World() = default;
  ~World() = default;

  Resources* resources() {
    return &resources_;
  }

  void clear();
  EntityId add_entity_from_prefab(Entity* prefab, const fl::Vec2& position);

  void set_cursor_position(const fl::Vec2& position);
  NU_NO_DISCARD EntityId get_entity_under_cursor() const;

  void tick(F32 delta);
  void render(ca::Renderer* renderer, le::Camera* camera);

private:
  fl::Vec2 cursor_position_ = fl::Vec2::zero;

  EntityList entities_;

  Resources resources_;

  MovementSystem movement_system_;
  ResourceSystem resource_system_{&resources_};
};
