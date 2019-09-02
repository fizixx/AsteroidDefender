#ifndef AD_GEOMETRY_TEXTURE_CONVERTER_H_
#define AD_GEOMETRY_TEXTURE_CONVERTER_H_

#include "ad/Geometry/Geometry.h"
#include "ad/Geometry/RendererConverter.h"

class TextureConverter : public RendererConverter<Texture> {
public:
  TextureConverter() = default;
  ~TextureConverter() override = default;

private:
  DELETE_COPY_AND_MOVE(TextureConverter);

  bool load(hi::ResourceManager* resourceManager, nu::InputStream* inputStream,
            Texture* storage) override;
};

#endif  // AD_GEOMETRY_TEXTURE_CONVERTER_H_
