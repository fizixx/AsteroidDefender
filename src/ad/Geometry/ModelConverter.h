#ifndef AD_GEOMETRY_SCENE_CONVERTER_H_
#define AD_GEOMETRY_SCENE_CONVERTER_H_

#include "ad/Geometry/Geometry.h"
#include "canvas/Renderer/Renderer.h"
#include "hive/Converter.h"
#include "silhouette/Import/Import.h"
#include "silhouette/Scene/Scene.h"

class ModelConverter : public hi::Converter<Model> {
public:
  ModelConverter() {
    m_vertexDefinition.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Three,
                                    "position");
  }

  void setRenderer(ca::Renderer* renderer) {
    m_renderer = renderer;
  }

private:
  bool load(hi::ResourceManager* resourceManager, nu::InputStream* inputStream,
            Model* model) override;

private:
  ca::Renderer* m_renderer = nullptr;
  ca::VertexDefinition m_vertexDefinition;
};

#endif  // AD_GEOMETRY_SCENE_CONVERTER_H_
