#ifndef AD_GEOMETRY_RENDERER_CONVERTER_H_
#define AD_GEOMETRY_RENDERER_CONVERTER_H_

#include "hive/Converter.h"

template <typename ResourceType>
class RendererConverter : public hi::Converter<ResourceType> {
public:
  RendererConverter() = default;

  void setRenderer(ca::Renderer* renderer) {
    m_renderer = renderer;
  }

protected:
  ca::Renderer* m_renderer = nullptr;

private:
  DELETE_COPY_AND_MOVE(RendererConverter);
};

#endif  // AD_GEOMETRY_RENDERER_CONVERTER_H_
