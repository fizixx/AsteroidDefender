#ifndef WORLD_H_
#define WORLD_H_

#include "ad/World/Camera.h"
#include "ad/Sprites/SpriteRenderer.h"

#include "canvas/Renderer/Renderer.h"
#include "canvas/Windows/Event.h"
#include "hive/ResourceManager.h"
#include "nucleus/FilePath.h"
#include "nucleus/Macros.h"

class World {
public:
  World() = default;
  ~World() = default;

  bool initialize(hi::ResourceManager* resourceManager);
  void tick(F32 delta);
  void render(SpriteRenderer* spriteRenderer);

private:
  DELETE_COPY_AND_MOVE(World);

  Sprite* m_cursorSprite = nullptr;
  Sprite* m_commandCenterSprite = nullptr;
};

#endif  // WORLD_H_
