#ifndef AD_GEOMETRY_CONVERTERS_H_
#define AD_GEOMETRY_CONVERTERS_H_

#include "hive/ResourceManager.h"
#include "reactor/Resources/Converters/ImageConverter.h"
#include "reactor/Resources/Converters/ModelConverter.h"
#include "reactor/Resources/Converters/ShaderSourceConverter.h"
#include "reactor/Resources/Converters/TextureConverter.h"

namespace ca {
class Renderer;
}

class Converters {
public:
  Converters() = default;

  void registerConverters(hi::ResourceManager* resourceManager, ca::Renderer* renderer) {
    resourceManager->registerConverter(&m_imageConverter);

    m_modelConverter.setRenderer(renderer);
    resourceManager->registerConverter(&m_modelConverter);

    m_textureConverter.setRenderer(renderer);
    resourceManager->registerConverter(&m_textureConverter);

    m_shaderSourceConverter.setRenderer(renderer);
    resourceManager->registerConverter(&m_shaderSourceConverter);
  }

private:
  DELETE_COPY_AND_MOVE(Converters);

  re::ImageConverter m_imageConverter;
  re::ModelConverter m_modelConverter;
  re::TextureConverter m_textureConverter;
  re::ShaderSourceConverter m_shaderSourceConverter;
};

#endif  // AD_GEOMETRY_CONVERTERS_H_
