#include "ad/Geometry/ModelConverter.h"

static void createMesh(ca::Renderer* renderer, const ca::VertexDefinition& vertexDefinition,
                       si::Mesh* src, Mesh* dst) {
  struct V {
    ca::Vec3 position;
    ca::Color color;
  };
  nu::DynamicArray<V> buffer{src->vertices.size()};
  for (ca::Vec3& v : src->vertices) {
    buffer.emplaceBack(v, ca::Color::red);
  }

  auto vertexBufferId =
      renderer->createVertexBuffer(vertexDefinition, buffer.data(), buffer.size() * sizeof(V));

  auto indexBufferId = renderer->createIndexBuffer(
      ca::ComponentType::Unsigned16, src->indices.data(), src->indices.size() * sizeof(U16));

  new (dst) Mesh{vertexBufferId, indexBufferId, static_cast<U32>(src->vertices.size()),
                 ca::DrawType::Triangles};
}

static void createNode(si::Node* src, Node* dst) {
  dst->transform = src->transform;
  for (auto& meshIndex : src->meshIndices) {
    dst->meshIndices.emplaceBack(meshIndex);
  }

  for (auto& childNode : src->children) {
    dst->children.emplace_back(ca::Mat4::identity);
    createNode(&childNode, &*dst->children.rbegin());
  }
}

ModelConverter::ModelConverter() {
  m_vertexDefinition.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Three,
                                  "position");
  m_vertexDefinition.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Four, "color");
}

bool ModelConverter::load(hi::ResourceManager* UNUSED(resourceManager),
                          nu::InputStream* inputStream, Model* model) {
  if (!m_renderer) {
    LOG(Error) << "Can not load geometry without a renderer.";
    return false;
  }

  si::Scene scene;
  if (!si::loadCollada(&scene, inputStream)) {
    LOG(Error) << "Could not load geometry ()";
    return false;
  }

  if (scene.meshes.isEmpty()) {
    LOG(Warning) << "Model contains no geometry.";
    return false;
  }

  // Add all the meshes.
  for (auto& sceneMesh : scene.meshes) {
    auto result = model->meshes.emplaceBack();
    createMesh(m_renderer, m_vertexDefinition, &sceneMesh, &result.element());
  }

  // Add nodes.
  createNode(&scene.rootNode, &model->rootNode);

  return true;
}
