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

bool createCube(Model* model, ca::Renderer* renderer) {
  ca::VertexDefinition def;
  def.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Three, "position");
  def.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Four, "color");

  auto vertexBufferId = renderer->createVertexBuffer(def, kCubeVertices, sizeof(kCubeVertices));
  if (!isValid(vertexBufferId)) {
    LOG(Error) << "Could not create cube vertices.";
    return false;
  }

  auto indexBufferId = renderer->createIndexBuffer(ca::ComponentType::Unsigned16, kCubeIndices,
                                                   sizeof(kCubeIndices));
  if (!isValid(indexBufferId)) {
    LOG(Error) << "Could not create cube indices.";
    return false;
  }

  model->meshes.emplaceBack(vertexBufferId, indexBufferId,
                            static_cast<U32>(sizeof(kCubeIndices) / sizeof(U16)),
                            ca::DrawType::Triangles);

  model->rootNode.meshIndices.emplaceBack(0U);

  return true;
}

static void renderNode(ca::Renderer* renderer, const Model& model, const Node& node,
                       const ca::Mat4& transform, ca::ProgramId programId,
                       ca::UniformId transformUniformId) {
  ca::Mat4 t = transform * node.transform;

  for (MemSize i = 0; i < node.meshIndices.size(); ++i) {
    const Mesh& mesh = model.meshes[node.meshIndices[i]];

    ca::UniformBuffer uniforms;
    uniforms.set(transformUniformId, t);
    renderer->draw(mesh.drawType, mesh.numIndices, programId, mesh.vertexBufferId,
                   mesh.indexBufferId, {}, uniforms);
  }

  for (const Node& childNode : node.children) {
    renderNode(renderer, model, childNode, t, programId, transformUniformId);
  }
}

void renderModel(ca::Renderer* renderer, const Model& model, const ca::Mat4& transform,
                 ca::ProgramId programId, ca::UniformId transformUniformId) {
  renderNode(renderer, model, model.rootNode, transform, programId, transformUniformId);
}
