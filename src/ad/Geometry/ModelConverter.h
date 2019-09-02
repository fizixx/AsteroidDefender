#ifndef AD_GEOMETRY_SCENE_CONVERTER_H_
#define AD_GEOMETRY_SCENE_CONVERTER_H_

#include "ad/Geometry/Geometry.h"
#include "ad/Geometry/RendererConverter.h"
#include "canvas/Renderer/Renderer.h"
#include "silhouette/Import/Import.h"
#include "silhouette/Scene/Scene.h"

class ModelConverter : public RendererConverter<Model> {
public:
  ModelConverter();

private:
  bool load(hi::ResourceManager* resourceManager, nu::InputStream* inputStream,
            Model* model) override;

  ca::VertexDefinition m_vertexDefinition;
};

#endif  // AD_GEOMETRY_SCENE_CONVERTER_H_
