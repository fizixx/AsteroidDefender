#ifndef AD_GEOMETRY_GEOMETRY_H_
#define AD_GEOMETRY_GEOMETRY_H_

#include "canvas/Math/Mat4.h"
#include "canvas/Renderer/Types.h"
#include "canvas/Utils/Color.h"
#include "canvas/Utils/Size.h"
#include "nucleus/Containers/DynamicArray.h"

#include <utility>
#include <vector>

namespace ca {

class Renderer;
class UniformBuffer;

}  // namespace ca

struct Texture {
  ca::TextureId textureId;
  ca::Size size;
};

enum class MaterialType : U32 {
  DiffuseColor,
  Textured,
};

struct Material {
  MaterialType type;

  ca::ProgramId programId;
  ca::UniformId transformUniformId;

  struct {
    ca::Color color = ca::Color::red;
    Texture* texture;
    ca::UniformId textureUniformId;
  } diffuse;
};

struct Mesh {
  MemSize materialIndex;
  ca::VertexBufferId vertexBufferId;
  ca::IndexBufferId indexBufferId;
  U32 numIndices;
  ca::DrawType drawType;

  Mesh() : materialIndex{0}, numIndices{0}, drawType{ca::DrawType::Triangles} {}
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
  nu::DynamicArray<Material> materials;
  Node rootNode;
};

bool createCube(Model* model, ca::Renderer* renderer);

void renderModel(ca::Renderer* renderer, const Model& model, const ca::Mat4& transform,
                 ca::ProgramId programId, ca::UniformId transformUniformId);

#endif  // AD_GEOMETRY_GEOMETRY_H_
