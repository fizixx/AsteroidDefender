#pragma once

#include "ad/World/Entity.h"
#include "ad/World/Prefabs.h"
#include "ad/World/World.h"
#include "floats/Transform.h"
#include "legion/Rendering/Rendering.h"
#include "legion/World/Camera.h"

class ConstructionController {
public:
  explicit ConstructionController(World* world, Prefabs* prefabs)
    : world_{world}, prefabs_{prefabs} {}

  auto start_building(EntityType entity_type) -> void {
    prefab_ = prefabs_->get(entity_type);
  }

  NU_NO_DISCARD auto is_building() const -> bool {
    return prefab_ != nullptr;
  }

  auto build() -> void {
    if (!prefab_) {
      return;
    }

    world_->add_entity_from_prefab(prefab_, cursor_position_);

    prefab_ = nullptr;
  }

  auto set_cursor_position(const fl::Vec2& cursor_position) -> void {
    cursor_position_ = cursor_position;
  }

  auto render(ca::Renderer* renderer, le::Camera* camera) -> void {
    if (!prefab_) {
      return;
    }

    fl::Mat4 projection{fl::Mat4::identity}, view{fl::Mat4::identity};

    camera->updateProjectionMatrix(&projection);
    camera->updateViewMatrix(&view);

    auto final = projection * view * fl::translationMatrix(fl::Vec3{cursor_position_, 0.0f});

    le::renderModel(renderer, *prefab_->render.model, final);
  }

private:
  World* world_;
  Prefabs* prefabs_;

  fl::Vec2 cursor_position_ = fl::Vec2::zero;
  Entity* prefab_ = nullptr;
};
