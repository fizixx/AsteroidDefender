#ifndef AD_GEOMETRY_GEOMETRY_H_
#define AD_GEOMETRY_GEOMETRY_H_

#include "reactor/Resources/Model.h"

namespace ca {

class Renderer;
class UniformBuffer;

}  // namespace ca

void renderModel(ca::Renderer* renderer, const re::Model& model, const ca::Mat4& transform);

#endif  // AD_GEOMETRY_GEOMETRY_H_
