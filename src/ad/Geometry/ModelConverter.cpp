#include "ad/Geometry/ModelConverter.h"

static void createMesh(ca::Renderer* renderer, const ca::VertexDefinition& vertexDefinition,
                       si::Mesh* src, Mesh* dst) {
  auto vertexBufferId = renderer->createVertexBuffer(vertexDefinition, src->vertices.getData(),
                                                     src->vertices.getAllocated());

  auto indexBufferId = renderer->createIndexBuffer(
      ca::ComponentType::Unsigned16, src->indices.getData(), src->indices.getAllocated());

  new (dst) Mesh{vertexBufferId, indexBufferId, static_cast<U32>(src->vertices.getSize()),
                 ca::DrawType::Triangles};
}

static void createNode(si::Node* src, Node* dst) {
  for (auto& meshIndex : src->meshIndices) {
    dst->meshIndices.emplaceBack(meshIndex);
  }

  for (auto& childNode : src->children) {
    LOG(Info) << "childNode = " << childNode.children.getSize();
    dst->children.emplace_back(ca::Mat4::identity);
    createNode(&childNode, &*dst->children.rbegin());
  }
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
    //    auto result = model->meshes.emplace_back();
    //     createMesh(m_renderer, m_vertexDefinition, &sceneMesh, &result.element());
    model->meshes.emplace_back();
    createMesh(m_renderer, m_vertexDefinition, &sceneMesh, &*model->meshes.rbegin());
  }

  // Add nodes.
  createNode(&scene.rootNode, &model->rootNode);

  return true;
}
