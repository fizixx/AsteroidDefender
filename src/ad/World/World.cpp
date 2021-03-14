#include "World.h"

#include "ad/World/Entity.h"
#include "canvas/Utils/ImmediateShapes.h"
#include "floats/Common.h"
#include "floats/Transform.h"
#include "hive/ResourceManager.h"
#include "legion/Rendering/Rendering.h"
#include "nucleus/Profiling.h"

auto World::clear() -> void {
  m_entities.clear();
}

auto World::addEntityFromPrefab(Entity* prefab, const fl::Vec2& position) -> EntityId {
  auto result = m_entities.pushBack(*prefab);
  result.element().position = position;
  return EntityId{result.index()};
}

void World::setCursorPosition(const fl::Vec2& position) {
  m_cursorPosition = position;
}

auto World::getEntityUnderCursor() const -> EntityId {
  // This is not a system right now, because the proper solution is probably not to go through each
  // entity to check.

  MemSize id = 0;
  for (const auto& entity : m_entities) {
    if (entity.building.selectionRadius == 0.0f) {
      continue;
    }

    F32 distanceToCursor = fl::length(entity.position - m_cursorPosition);
    if (distanceToCursor < entity.building.selectionRadius) {
      return EntityId{id};
    }
    ++id;
  }

  return EntityId{};
}

void World::tick(F32 delta) {
  m_resourceSystem.tick(m_entities, delta);
  m_movementSystem.tick(m_entities, delta);
}

void World::render(ca::Renderer* renderer, le::Camera* camera) {
  fl::Mat4 projection = fl::Mat4::identity;
  camera->updateProjectionMatrix(&projection);

  fl::Mat4 view = fl::Mat4::identity;
  camera->updateViewMatrix(&view);

  auto projectionAndView = projection * view;

  // Render the entities.

  ca::ImmediateRenderer immediate{renderer};
  immediate.setTransform(projectionAndView);

  for (auto& entity : m_entities) {
    PROFILE("item")

    auto translation = fl::translationMatrix(fl::Vec3{entity.position, 0.0f});
    auto rotation = fl::rotationMatrix(fl::Vec3{0.0f, 0.0f, 1.0f}, entity.movement.direction);

    auto mvp = projectionAndView * fl::createModelMatrix(translation, rotation, fl::Mat4::identity);

    // Draw the entity circle.

    if (entity.building.selectionRadius > 0.0f) {
      ca::drawCircle(&immediate, fl::Vec3{entity.position, 0.0f}, entity.building.selectionRadius,
                     16, ca::Color::red);
    }

    // Draw entity model.

    le::renderModel(renderer, *entity.render.model, mvp);
  }
}
