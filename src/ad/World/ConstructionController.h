#ifndef AD_WORLD_CONSTRUCTION_CONTROLLER_H_
#define AD_WORLD_CONSTRUCTION_CONTROLLER_H_

#include "ad/World/Entity.h"
#include "ad/World/Prefabs.h"
#include "ad/World/World.h"
#include "floats/Transform.h"
#include "legion/Rendering/Rendering.h"
#include "legion/World/Camera.h"

class ConstructionController {
public:
  explicit ConstructionController(World* world, Prefabs* prefabs)
    : m_world{world}, m_prefabs{prefabs} {}

  auto startBuilding(EntityType entityType) -> void {
    m_prefab = m_prefabs->get(entityType);
  }

  auto isBuilding() const -> bool {
    return m_prefab != nullptr;
  }

  auto build() -> void {
    if (!m_prefab) {
      return;
    }

    m_world->addEntityFromPrefab(m_prefab, m_cursorPosition);

    m_prefab = nullptr;
  }

  auto setCursorPosition(const fl::Vec2& cursorPosition) -> void {
    m_cursorPosition = cursorPosition;
  }

  auto render(ca::Renderer* renderer, le::Camera* camera) -> void {
    if (!m_prefab) {
      return;
    }

    fl::Mat4 projection{fl::Mat4::identity}, view{fl::Mat4::identity};

    camera->updateProjectionMatrix(&projection);
    camera->updateViewMatrix(&view);

    auto final = projection * view * fl::translationMatrix(fl::Vec3{m_cursorPosition, 0.0f});

    le::renderModel(renderer, *m_prefab->render.model, final);
  }

private:
  World* m_world;
  Prefabs* m_prefabs;

  fl::Vec2 m_cursorPosition{fl::Vec2::zero};
  Entity* m_prefab = nullptr;
};

#endif  // AD_WORLD_CONSTRUCTION_CONTROLLER_H_
