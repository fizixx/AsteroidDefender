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
  World() = default;
  ~World() = default;

  auto resources() -> Resources* {
    return &m_resources;
  }

  bool initialize(hi::ResourceManager* resourceManager);
  void generate();

  void setCursorPosition(const ca::Vec2& position);

  void tick(F32 delta);
  void render(ca::Renderer* renderer, le::Camera* camera);

  void startBuilding(BuildingType buildingType);
  void build();

private:
  DELETE_COPY_AND_MOVE(World);

  bool loadModels(hi::ResourceManager* resourceManager);

  EntityId createCommandCenter(const ca::Vec2& position);
  EntityId createMiner(const ca::Vec2& position);
  EntityId createAsteroid(const ca::Vec2& position);
  EntityId createEnemy(const ca::Vec2& position);

  struct {
    le::Model* commandCenter = nullptr;
    le::Model* miner = nullptr;
    le::Model* asteroid = nullptr;
    le::Model* enemy = nullptr;
  } m_models;

  struct {
    bool isBuilding = false;
    BuildingType buildingType = BuildingType::Unknown;
    le::Model* model = nullptr;
  } m_building;

  ca::Vec2 m_cursorPosition{ca::Vec2::zero};

  EntityList m_entities;

  Resources m_resources;

  MovementSystem m_movementSystem;
  ResourceSystem m_resourceSystem{&m_resources};
};

#endif  // WORLD_H_
