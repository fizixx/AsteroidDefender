#include "ad/Geometry/Geometry.h"

#include "canvas/Renderer/Renderer.h"

static void renderNode(ca::Renderer* renderer, const re::Model& model, const re::Node& node,
                       const ca::Mat4& transform) {
  ca::Mat4 t = transform * node.transform;

  for (auto meshIndex : node.meshIndices) {
    const re::Mesh& mesh = model.meshes[meshIndex];
    const re::Material& material = model.materials[mesh.materialIndex];

    // The material here might be null.

    // Set up the material.
    ca::UniformBuffer uniforms;
    uniforms.set(material.transformUniformId, t);

    if (material.type == re::MaterialType::Textured) {
      renderer->draw(mesh.drawType, mesh.vertexCount, material.programId, mesh.vertexBufferId,
                     material.diffuse.texture->id, uniforms);
    } else {
      renderer->draw(mesh.drawType, mesh.vertexCount, material.programId, mesh.vertexBufferId, {},
                     uniforms);
    }
  }

  for (const re::Node& childNode : node.children) {
    renderNode(renderer, model, childNode, t);
  }
}

void renderModel(ca::Renderer* renderer, const re::Model& model, const ca::Mat4& transform) {
  renderNode(renderer, model, model.rootNode, transform);
}
