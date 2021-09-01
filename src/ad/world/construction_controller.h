#pragma once

#include <floats/transform.h>
#include <legion/rendering/rendering.h>
#include <legion/world/camera.h>

#include "ad/world/entity.h"
#include "ad/world/prefabs.h"
#include "ad/world/world.h"

namespace ad {

class ConstructionController {
public:
  explicit ConstructionController(World* world, Prefabs* prefabs)
    : world_{world}, prefabs_{prefabs} {}

  void start_building(EntityType entity_type) {
    prefab_ = prefabs_->get(entity_type);
  }

  void cancel_building() {
    prefab_ = nullptr;
  }

  NU_NO_DISCARD bool is_building() const {
    return prefab_ != nullptr;
  }

  NU_NO_DISCARD Entity* prefab() const {
    return prefab_;
  }

  void build() {
    if (!prefab_) {
      return;
    }

    world_->add_entity_from_prefab(prefab_, cursor_position_);

    prefab_ = nullptr;
  }

  NU_NO_DISCARD const fl::Vec2& cursor_position() const {
    return cursor_position_;
  }

  void set_cursor_position(const fl::Vec2& cursor_position) {
    cursor_position_ = cursor_position;
  }

private:
  World* world_;
  Prefabs* prefabs_;

  fl::Vec2 cursor_position_ = fl::Vec2::zero;
  Entity* prefab_ = nullptr;
};

}  // namespace ad
