#ifndef AD_GEOMETRY_GEOMETRY_H_
#define AD_GEOMETRY_GEOMETRY_H_

#include "canvas/Renderer/Types.h"

namespace ca {

class Renderer;
class UniformBuffer;

}  // namespace ca

struct Geometry {
  ca::VertexBufferId vertexBufferId;
  ca::IndexBufferId indexBufferId;
  U32 numIndices;
  ca::DrawType drawType;
};

Geometry createCube(ca::Renderer* renderer);

void renderGeometry(ca::Renderer* renderer, const Geometry& geometry, ca::ProgramId programId,
                    const ca::UniformBuffer& uniforms);

#endif  // AD_GEOMETRY_GEOMETRY_H_
