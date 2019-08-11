#ifndef WORLD_H_
#define WORLD_H_

#include "ad/Sprites/SpriteRenderer.h"
#include "ad/World/Camera.h"
#include "ad/World/Entity.h"
#include "canvas/Renderer/Renderer.h"
#include "canvas/Windows/Event.h"
#include "hive/ResourceManager.h"
#include "nucleus/FilePath.h"
#include "nucleus/Macros.h"

class World {
public:
  World() = default;
  ~World() = default;

  void setCursorPosition(const ca::Vec2& position);

  bool initialize(hi::ResourceManager* resourceManager);
  void tick(F32 delta);
  void render(SpriteRenderer* spriteRenderer);

private:
  DELETE_COPY_AND_MOVE(World);

  ca::Vec2 m_cursorPosition{0.0f, 0.0f};

  Sprite* m_cursorSprite = nullptr;
  Sprite* m_commandCenterSprite = nullptr;

  EntityId m_cursorEntityId = kInvalidEntityId;

  nu::DynamicArray<Entity> m_entities;
};

#endif  // WORLD_H_
