#include "World.h"

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

void World::setCursorPosition(const ca::Vec2& position) {
  m_cursorPosition = position;
}

void World::tick(F32 delta) {
}

void World::render(SpriteRenderer* spriteRenderer) {
#if 0
  static F32 pos = 0.0f;
  static F32 delta = 1.0f;
  // pos += delta * 0.1f;
  if (pos >= 100.0f || pos <= -100.0f) {
    delta *= -1.0f;
  }
  spriteRenderer->renderSprite(m_commandCenterSprite, {pos, 0.0f}, 100.0f);

  spriteRenderer->renderSprite(m_cursorSprite, m_cursorPosition, 50.0f);
#endif  // 0

  for (Entity& entity : m_entities) {
    spriteRenderer->renderSprite(entity.sprite, entity.position, entity.scale);
  }
}
