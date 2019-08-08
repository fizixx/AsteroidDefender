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

  void onMouseMoved(const ca::MouseEvent& evt);
  bool onMousePressed(const ca::MouseEvent& evt);
  void onMouseReleased(const ca::MouseEvent& evt);
  void onMouseWheel(const ca::MouseWheelEvent& evt);
  void onKeyPressed(const ca::KeyEvent& evt);
  void onKeyReleased(const ca::KeyEvent& evt);

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
