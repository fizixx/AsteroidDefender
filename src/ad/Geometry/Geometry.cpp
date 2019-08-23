#include "ad/Geometry/Geometry.h"

#include "canvas/Math/Vec3.h"
#include "canvas/Renderer/Renderer.h"

namespace {

struct VertexAndColor {
  ca::Vec3 position;
  ca::Color color;
} kCubeVertices[] = {
    {{-0.5f, -0.5f, 0.5f}, {1.0, 0.0, 0.0}},   //
    {{0.5f, -0.5f, 0.5f}, {0.0, 1.0, 0.0}},    //
    {{0.5f, 0.5f, 0.5f}, {0.0, 0.0, 1.0}},     //
    {{-0.5f, 0.5f, 0.5f}, {1.0, 1.0, 1.0}},    //
    {{-0.5f, -0.5f, -0.5f}, {1.0, 0.0, 0.0}},  //
    {{0.5f, -0.5f, -0.5f}, {0.0, 1.0, 0.0}},   //
    {{0.5f, 0.5f, -0.5f}, {0.0, 0.0, 1.0}},    //
    {{-0.5f, 0.5f, -0.5f}, {1.0, 1.0, 1.0}},   //
};

U16 kCubeIndices[] = {
    // front
    0, 1, 2,  //
    2, 3, 0,  //
    // right
    1, 5, 6,  //
    6, 2, 1,  //
    // back
    7, 6, 5,  //
    5, 4, 7,  //
    // left
    4, 0, 3,  //
    3, 7, 4,  //
    // bottom
    4, 5, 1,  //
    1, 0, 4,  //
    // top
    3, 2, 6,  //
    6, 7, 3,  //
};

}  // namespace

Geometry createCube(ca::Renderer* renderer) {
  ca::VertexDefinition def;
  def.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Three, "position");
  def.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Four, "color");

  auto vertexBufferId = renderer->createVertexBuffer(def, kCubeVertices, sizeof(kCubeVertices));
  if (!isValid(vertexBufferId)) {
    LOG(Error) << "Could not create cube vertices.";
    return {};
  }

  auto indexBufferId = renderer->createIndexBuffer(ca::ComponentType::Unsigned16, kCubeIndices,
                                                   sizeof(kCubeIndices));
  if (!isValid(indexBufferId)) {
    LOG(Error) << "Could not create cube indices.";
    return {};
  }

  return {vertexBufferId, indexBufferId, sizeof(kCubeIndices) / sizeof(U16),
          ca::DrawType::Triangles};
}

void renderGeometry(ca::Renderer* renderer, const Geometry& geometry, ca::ProgramId programId,
                    const ca::UniformBuffer& uniforms) {
  renderer->draw(geometry.drawType, geometry.numIndices, programId, geometry.vertexBufferId,
                 geometry.indexBufferId, {}, uniforms);
}
