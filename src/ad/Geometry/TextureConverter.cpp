#include "ad/Geometry/TextureConverter.h"

#include "canvas/Renderer/Renderer.h"
#include "nucleus/Streams/InputStream.h"
#include "silhouette/Image/Image.h"

bool TextureConverter::load(hi::ResourceManager* UNUSED(resourceManager),
                            nu::InputStream* inputStream, Texture* storage) {
  DCHECK(m_renderer) << "Can only convert textures with a renderer.";

  si::Image image;
  if (!image.loadFromStream(inputStream)) {
    LOG(Error) << "Could not load image from stream.";
    return false;
  }

  auto textureId = si::createTextureFromImage(m_renderer, image, false);
  if (!textureId.isValid()) {
    LOG(Error) << "Could not create texture from image.";
    return false;
  }

  storage->textureId = textureId;
  storage->size = image.size();

  return true;
}
