#include "ad/Sprites/SpriteConverter.h"

#include "canvas/Renderer/Renderer.h"

SpriteConverter::SpriteConverter() : m_renderer{nullptr} {}

SpriteConverter::SpriteConverter(ca::Renderer* renderer) : m_renderer{renderer} {}

void SpriteConverter::setRenderer(ca::Renderer* renderer) {
  m_renderer = renderer;
}

bool SpriteConverter::load(hi::ResourceManager* UNUSED(resourceManager),
                           nu::InputStream* inputStream, Sprite* storage) {
  si::Image image;
  if (!image.loadFromStream(inputStream)) {
    return false;
  }

  ca::TextureId textureId = si::createTextureFromImage(m_renderer, image, false);
  if (!isValid(textureId)) {
    return false;
  }

  storage->size = image.size();
  storage->textureId = textureId;

  return true;
}
