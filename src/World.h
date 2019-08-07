#ifndef WORLD_H_
#define WORLD_H_

#include "Camera.h"
#include "SpriteRenderer.h"

#include "canvas/Renderer/Renderer.h"
#include "nucleus/FilePath.h"
#include "nucleus/Macros.h"
#include "hive/ResourceManager.h"

class World {
public:
  World() = default;
  ~World() = default;

  bool initialize(ca::Renderer* renderer, hi::ResourceManager* resourceManager);

  // Set the cursor position in device coordinates.
  void setCursorPosition(const ca::Pos& pos);

  void render(SpriteRenderer* spriteRenderer);

private:
  DELETE_COPY_AND_MOVE(World);

  Sprite* m_cursorSprite = nullptr;
  Sprite* m_commandCenterSprite = nullptr;

  Camera m_camera;

  ca::Pos m_cursorPosition;
};

#endif  // WORLD_H_
