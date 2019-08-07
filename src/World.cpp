#include "World.h"

#include "canvas/Utils/Image.h"
#include "nucleus/Streams/FileInputStream.h"

namespace {

ca::TextureId loadTexture(ca::Renderer* renderer, const nu::FilePath& assetsPath,
                          const nu::StringView& name) {
  auto path = assetsPath / name;

  nu::FileInputStream fis{path};
  ca::Image image;
  if (!image.loadFromStream(&fis)) {
    return {};
  }

  return renderer->createTexture(image);
}

}  // namespace

bool World::initialize(ca::Renderer* renderer, hi::ResourceManager* resourceManager) {
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
