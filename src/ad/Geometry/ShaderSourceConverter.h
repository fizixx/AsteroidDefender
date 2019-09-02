#ifndef AD_GEOMETRY_PROGRAM_CONVERTER_H_
#define AD_GEOMETRY_PROGRAM_CONVERTER_H_

#include "ad/Geometry/Geometry.h"
#include "ad/Geometry/RendererConverter.h"
#include "canvas/Utils/ShaderSource.h"

class ShaderSourceConverter : public RendererConverter<ca::ShaderSource> {
public:
  ShaderSourceConverter() = default;
  ~ShaderSourceConverter() override = default;

private:
  DELETE_COPY_AND_MOVE(ShaderSourceConverter);

  bool load(hi::ResourceManager* resourceManager, nu::InputStream* inputStream,
            ca::ShaderSource* storage) override;
};

#endif  // AD_GEOMETRY_PROGRAM_CONVERTER_H_
