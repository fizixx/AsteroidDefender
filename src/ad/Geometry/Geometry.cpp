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

  auto result = model->meshes.emplaceBack();
  Mesh* mesh = &result.element();

  mesh->materialIndex = 0;
  mesh->vertexBufferId = vertexBufferId;
  mesh->indexBufferId = indexBufferId;
  mesh->numIndices = static_cast<U32>(sizeof(kCubeIndices) / sizeof(U16));
  mesh->drawType = ca::DrawType::Triangles;

  model->rootNode.meshIndices.emplaceBack(0U);

  return true;
}

static void renderNode(ca::Renderer* renderer, const Model& model, const Node& node,
                       const ca::Mat4& transform, ca::ProgramId programId,
                       ca::UniformId transformUniformId) {
  ca::Mat4 t = transform * node.transform;

  for (auto meshIndex : node.meshIndices) {
    const Mesh& mesh = model.meshes[meshIndex];
    const Material& material = model.materials[mesh.materialIndex];

    // The material here might be null.

    // Set up the material.
    ca::UniformBuffer uniforms;
    uniforms.set(material.transformUniformId, t);

    if (material.type == MaterialType::Textured) {
      renderer->draw(mesh.drawType, mesh.numIndices, material.programId, mesh.vertexBufferId,
                     mesh.indexBufferId, material.diffuse.texture->textureId, uniforms);
    } else {
      renderer->draw(mesh.drawType, mesh.numIndices, material.programId, mesh.vertexBufferId,
                     mesh.indexBufferId, {}, uniforms);
    }
  }

  for (const Node& childNode : node.children) {
    renderNode(renderer, model, childNode, t, programId, transformUniformId);
  }
}

void renderModel(ca::Renderer* renderer, const Model& model, const ca::Mat4& transform,
                 ca::ProgramId programId, ca::UniformId transformUniformId) {
  renderNode(renderer, model, model.rootNode, transform, programId, transformUniformId);
}
