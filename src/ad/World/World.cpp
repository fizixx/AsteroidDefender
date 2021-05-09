#include "ad/World/World.h"

#include "ad/World/Entity.h"
#include "canvas/Utils/ImmediateShapes.h"
#include "floats/Transform.h"
#include "legion/Rendering/Rendering.h"
#include "nucleus/Profiling.h"

void World::clear() {
  entities_.clear();
}

EntityId World::add_entity_from_prefab(Entity* prefab, const fl::Vec2& position) {
  auto new_id = entities_.size();
  auto result = entities_.emplace_back(*prefab);
  result.position = position;
  return EntityId{new_id};
}

void World::set_cursor_position(const fl::Vec2& position) {
  cursor_position_ = position;
}

EntityId World::get_entity_under_cursor() const {
  // This is not a system right now, because the proper solution is probably not to go through each
  // entity to check.

  MemSize id = 0;
  for (const auto& entity : entities_) {
    if (entity.building.selection_radius == 0.0f) {
      continue;
    }

    F32 distance_to_cursor = fl::length(entity.position - cursor_position_);
    if (distance_to_cursor < entity.building.selection_radius) {
      return EntityId{id};
    }
    ++id;
  }

  return EntityId{};
}

void World::tick(F32 delta) {
  resource_system_.tick(entities_, delta);
  movement_system_.tick(entities_, delta);
}

void World::render(ca::Renderer* renderer, le::Camera* camera) {
  fl::Mat4 projection = fl::Mat4::identity;
  camera->updateProjectionMatrix(&projection);

  fl::Mat4 view = fl::Mat4::identity;
  camera->updateViewMatrix(&view);

  auto projection_and_view = projection * view;

  // Render the entities.

  ca::ImmediateRenderer immediate{renderer};

  for (auto& entity : entities_) {
    PROFILE("item")

    auto translation = fl::translationMatrix(fl::Vec3{entity.position, 0.0f});
    auto rotation = fl::rotationMatrix(fl::Vec3{0.0f, 0.0f, 1.0f}, entity.movement.direction);

    auto mvp =
        projection_and_view * fl::createModelMatrix(translation, rotation, fl::Mat4::identity);

    // Draw the entity circle.

    // TODO: set projection_and_view
    if (entity.building.selection_radius > 0.0f) {
      ca::drawCircle(&immediate, fl::Vec3{entity.position, 0.0f}, entity.building.selection_radius,
                     16, ca::Color::red);
    }

    // Draw entity model.

    le::renderModel(renderer, *entity.render.model, mvp);
  }
}
