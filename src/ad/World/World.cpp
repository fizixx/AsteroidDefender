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

  m_cursorEntityId = createCursor();
  createCommandCenter({0.0f, 0.0f});
  createCommandCenter({20.0f, 0.0f});
  createCommandCenter({-20.0f, 0.0f});
  createCommandCenter({0.0f, 20.0f});
  createCommandCenter({0.0f, -20.0f});

  return true;
}

void World::setCursorPosition(const ca::Vec2& position) {
  // LOG(Info) << "new cursor position: " << position;
   m_cursorPosition = position;
}

void World::tick(F32 UNUSED(delta)) {
  m_entities[m_cursorEntityId].position = m_cursorPosition;
}

void World::render(SpriteRenderer* spriteRenderer) {
  for (Entity& entity : m_entities) {
    spriteRenderer->renderSprite(entity.sprite, entity.position, entity.scale);
  }
}

EntityId World::createCursor() {
  auto result = m_entities.pushBack([this](Entity* entity) {
    entity->position = {0.0f, 0.0f};
    entity->scale = 5.0f;
    entity->sprite = m_cursorSprite;
  });

  return result.index();
}

EntityId World::createCommandCenter(const ca::Vec2& position) {
  auto result = m_entities.pushBack([this, &position](Entity* entity) {
    entity->position = position;
    entity->scale = 10.0f;
    entity->sprite = m_commandCenterSprite;
  });

  return result.index();
}
