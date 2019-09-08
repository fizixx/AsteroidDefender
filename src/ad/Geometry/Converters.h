#ifndef AD_GEOMETRY_CONVERTERS_H_
#define AD_GEOMETRY_CONVERTERS_H_

#include "ad/Geometry/ModelConverter.h"
#include "ad/Geometry/ShaderSourceConverter.h"
#include "ad/Geometry/TextureConverter.h"
#include "hive/ResourceManager.h"

namespace ca {
class Renderer;
}

class Converters {
public:
  Converters() = default;

  void registerConverters(hi::ResourceManager* resourceManager, ca::Renderer* renderer) {
    m_modelConverter.setRenderer(renderer);
    resourceManager->registerConverter(&m_modelConverter);

    m_textureConverter.setRenderer(renderer);
    resourceManager->registerConverter(&m_textureConverter);

    m_shaderSourceConverter.setRenderer(renderer);
    resourceManager->registerConverter(&m_shaderSourceConverter);
  }

private:
  DELETE_COPY_AND_MOVE(Converters);

  ModelConverter m_modelConverter;
  TextureConverter m_textureConverter;
  ShaderSourceConverter m_shaderSourceConverter;
};

#endif  // AD_GEOMETRY_CONVERTERS_H_
