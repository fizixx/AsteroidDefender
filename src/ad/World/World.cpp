#include "ad/World/World.h"

#include "canvas/Utils/Image.h"
#include "nucleus/Streams/FileInputStream.h"

bool World::initialize(hi::ResourceManager* resourceManager) {
  m_cursorSprite = resourceManager->get<Sprite>("cursor.png");
  if (!m_cursorSprite) {
    LOG(Error) << "Could not load cursor sprite.";
    return false;
  }

  m_commandCenterSprite = resourceManager->get<Sprite>("command_center.png");
  if (!m_commandCenterSprite) {
    LOG(Error) << "Could not load command center sprite.";
    return false;
  }

  return true;
}

void World::onMouseMoved(const ca::MouseEvent& evt) {
  setCursorPosition(evt.pos);
}

bool World::onMousePressed(const ca::MouseEvent& UNUSED(evt)) {
  return false;
}

void World::onMouseReleased(const ca::MouseEvent& UNUSED(evt)) {}

void World::onMouseWheel(const ca::MouseWheelEvent& evt) {
  m_camera.zoom += -evt.wheelOffset.y * 10.0f;
}

void World::onKeyPressed(const ca::KeyEvent& evt) {
  switch (evt.key) {
    case ca::Key::A:
      m_camera.position.x -= 1.0f;
      break;

    case ca::Key::D:
      m_camera.position.x += 1.0f;
      break;

    case ca::Key::W:
      m_camera.position.y += 1.0f;
      break;

    case ca::Key::S:
      m_camera.position.y -= 1.0f;
      break;

    default:
      break;
  }
}

void World::onKeyReleased(const ca::KeyEvent& UNUSED(evt)) {}

void World::setCursorPosition(const ca::Pos& pos) {
  LOG(Info) << "Cursor position = (" << pos.x << ", " << pos.y << ")";
  m_cursorPosition = pos;
}

void World::render(SpriteRenderer* spriteRenderer) {
  spriteRenderer->beginFrame(&m_camera);

  static F32 pos = 0.0f;
  static F32 delta = 1.0f;
  pos += delta * 0.1f;
  if (pos >= 100.0f || pos <= -100.0f) {
    delta *= -1.0f;
  }
  spriteRenderer->renderSprite(m_commandCenterSprite, {pos, 0.0f}, 100.0f);

  // Calculate the cursor position in world coordinates.
  auto cursorPositionInWorld =
      spriteRenderer->convertDeviceCoordinatesToWorldCoordinates(m_cursorPosition);

  spriteRenderer->renderSprite(m_cursorSprite, cursorPositionInWorld, 100.0f);
}
