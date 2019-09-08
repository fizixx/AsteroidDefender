#ifndef WORLD_H_
#define WORLD_H_

#include "ad/World/Entity.h"
#include "nucleus/Containers/DynamicArray.h"
#include "nucleus/Macros.h"

namespace ca {
class Renderer;
}

namespace hi {
class ResourceManager;
}

class Camera;

class World {
public:
  World() = default;
  ~World() = default;

  bool initialize(hi::ResourceManager* resourceManager);
  void generate();

  void setCursorPosition(const ca::Vec2& position);

  void tick(F32 delta);
  void render(ca::Renderer* renderer, Camera* camera);

private:
  DELETE_COPY_AND_MOVE(World);

  bool loadModels(hi::ResourceManager* resourceManager);

  EntityId createCommandCenter();
  EntityId createMiner(const ca::Vec2& position);
  EntityId createAsteroid(const ca::Vec2& position);
  EntityId createEnemy(const ca::Vec2& position);

  struct {
    Model* commandCenter = nullptr;
    Model* miner = nullptr;
    Model* asteroid = nullptr;
    Model* enemy = nullptr;
  } m_models;

  ca::Vec2 m_cursorPosition{ca::Vec2::zero};

  nu::DynamicArray<Entity> m_entities;
};

#endif  // WORLD_H_
