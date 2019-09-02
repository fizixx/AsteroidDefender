#ifndef AD_GEOMETRY_GEOMETRY_H_
#define AD_GEOMETRY_GEOMETRY_H_

#include "canvas/Math/Mat4.h"
#include "canvas/Renderer/Types.h"
#include "nucleus/Containers/DynamicArray.h"

#include <utility>
#include <vector>

namespace ca {

class Renderer;
class UniformBuffer;

}  // namespace ca

struct Mesh {
  ca::VertexBufferId vertexBufferId;
  ca::IndexBufferId indexBufferId;
  U32 numIndices;
  ca::DrawType drawType;

  Mesh() : numIndices{0}, drawType{ca::DrawType::Triangles} {}

  Mesh(ca::VertexBufferId vertexBufferId, ca::IndexBufferId indexBufferId, U32 numIndices,
       ca::DrawType drawType)
    : vertexBufferId{std::move(vertexBufferId)},
      indexBufferId{std::move(indexBufferId)},
      numIndices{numIndices},
      drawType{drawType} {}
};

struct Node {
  ca::Mat4 transform;
  nu::DynamicArray<MemSize> meshIndices;
  std::vector<Node> children;

  Node() : transform(ca::Mat4::identity) {}
  explicit Node(const ca::Mat4& transform) : transform{transform} {}
};

struct Model {
  nu::DynamicArray<Mesh> meshes;
  Node rootNode;
};

bool createCube(Model* model, ca::Renderer* renderer);

void renderModel(ca::Renderer* renderer, const Model& model, const ca::Mat4& transform,
                 ca::ProgramId programId, ca::UniformId transformUniformId);

#endif  // AD_GEOMETRY_GEOMETRY_H_
