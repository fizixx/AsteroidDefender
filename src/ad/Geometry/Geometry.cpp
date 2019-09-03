#include "ad/Geometry/Geometry.h"

#include "canvas/Renderer/Renderer.h"

static void renderNode(ca::Renderer* renderer, const Model& model, const Node& node,
                       const ca::Mat4& transform) {
  ca::Mat4 t = transform * node.transform;

  for (auto meshIndex : node.meshIndices) {
    const Mesh& mesh = model.meshes[meshIndex];
    const Material& material = model.materials[mesh.materialIndex];

    // The material here might be null.

    // Set up the material.
    ca::UniformBuffer uniforms;
    uniforms.set(material.transformUniformId, t);

    if (material.type == MaterialType::Textured) {
      renderer->draw(mesh.drawType, mesh.vertexCount, material.programId, mesh.vertexBufferId,
                     material.diffuse.texture->textureId, uniforms);
    } else {
      renderer->draw(mesh.drawType, mesh.vertexCount, material.programId, mesh.vertexBufferId, {},
                     uniforms);
    }
  }

  for (const Node& childNode : node.children) {
    renderNode(renderer, model, childNode, t);
  }
}

void renderModel(ca::Renderer* renderer, const Model& model, const ca::Mat4& transform) {
  renderNode(renderer, model, model.rootNode, transform);
}
