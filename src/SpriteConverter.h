#ifndef SPRITE_CONVERTER_H_
#define SPRITE_CONVERTER_H_

#include "Sprite.h"
#include "canvas/Utils/Image.h"
#include "hive/Converter.h"

namespace ca {
class Renderer;
}

class SpriteConverter : public hi::Converter<Sprite> {
public:
  SpriteConverter();
  explicit SpriteConverter(ca::Renderer* renderer);

  void setRenderer(ca::Renderer* renderer);

  bool load(hi::ResourceManager* resourceManager, nu::InputStream* inputStream,
            Sprite* storage) override;

private:
  DELETE_COPY_AND_MOVE(SpriteConverter);

  ca::Renderer* m_renderer;
};

#endif  // SPRITE_CONVERTER_H_
