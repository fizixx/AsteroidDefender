#include "ad/Sprites/SpriteRenderer.h"

#include "canvas/Math/Transform.h"
#include "nucleus/Streams/WrappedMemoryInputStream.h"

namespace {

const I8* kQuadVertexShaderSource = R"(
#version 330

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

out vec2 texCoords;

uniform mat4 uTransform;
uniform mat4 uTexCoordsTransform;

void main() {
  texCoords = (uTexCoordsTransform * vec4(inTexCoords, 0.0, 1.0)).xy;
  gl_Position = uTransform * vec4(inPosition, -0.5, 1.0);
}
)";

const I8* kQuadFragmentShaderSource = R"(
#version 330

in vec2 texCoords;

out vec4 final;

uniform sampler2D uTexture;

void main() {
  final = texture(uTexture, texCoords);
}
)";

struct QuadVertex {
  ca::Vec2 position;
  ca::Vec2 texCoords;
} kQuadVertices[] = {
    {{-0.5f, -0.5f}, {0.0f, 0.0f}},  //
    {{-0.5f, +0.5f}, {0.0f, 1.0f}},  //
    {{+0.5f, +0.5f}, {1.0f, 1.0f}},  //
    {{+0.5f, -0.5f}, {1.0f, 0.0f}},  //
};

U8 kQuadIndices[] = {
    0, 1, 2,  //
    2, 3, 0,  //
};

ca::ProgramId createProgram(ca::Renderer* renderer, const I8* vertexShaderSource,
                            const I8* fragmentShaderSource) {
  nu::WrappedMemoryInputStream vertexStream{vertexShaderSource,
                                            nu::StringView{vertexShaderSource}.getLength()};
  auto vss = ca::ShaderSource::from(&vertexStream);

  nu::WrappedMemoryInputStream fragmentStream{fragmentShaderSource,
                                              nu::StringView{fragmentShaderSource}.getLength()};
  auto fss = ca::ShaderSource::from(&fragmentStream);

  return renderer->createProgram(vss, fss);
}

}  // namespace

bool SpriteRenderer::initialize(ca::Renderer* renderer) {
  m_renderer = renderer;

  m_quad.programId = createProgram(renderer, kQuadVertexShaderSource, kQuadFragmentShaderSource);
  if (!isValid(m_quad.programId)) {
    LOG(Error) << "Could not create quad color shader program.";
    return false;
  }

  ca::VertexDefinition definition;
  definition.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Two, "inPosition");
  definition.addAttribute(ca::ComponentType::Float32, ca::ComponentCount::Two, "inTexCoords");

  m_quad.vertexBufferId =
      renderer->createVertexBuffer(definition, kQuadVertices, sizeof(kQuadVertices));
  if (!isValid(m_quad.vertexBufferId)) {
    LOG(Error) << "Could not create quad vertex buffer.";
    return false;
  }

  m_quad.indexBufferId =
      renderer->createIndexBuffer(ca::ComponentType::Unsigned8, kQuadIndices, sizeof(kQuadIndices));
  if (!isValid(m_quad.indexBufferId)) {
    LOG(Error) << "Could not create quad index buffer.";
    return false;
  }

  m_quad.transformUniformId = renderer->createUniform("uTransform");
  if (!isValid(m_quad.transformUniformId)) {
    LOG(Error) << "Could not create quad transform uniform.";
    return false;
  }

  m_quad.textureTransformUniformId = renderer->createUniform("uTexCoordsTransform");
  if (!isValid(m_quad.textureTransformUniformId)) {
    LOG(Error) << "Could not create quad tex coords transform uniform.";
    return false;
  }

  return true;
}

void SpriteRenderer::resize(const ca::Size& size) {
  F32 halfWidth = static_cast<F32>(size.width) / 2.0f;
  F32 halfHeight = static_cast<F32>(size.height) / 2.0f;

#if 0
  m_projection =
      ca::orthographicProjection(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
#else
  m_projection = ca::perspectiveProjection(60.0f, halfWidth / halfHeight, 0.1f, 1000.0f);
#endif
}

ca::Vec2 SpriteRenderer::convertDeviceCoordinatesToWorldCoordinates(ca::Pos pos) {
  auto result = m_view * ca::Vec4{static_cast<F32>(pos.x), static_cast<F32>(pos.y), 0.0f, 1.0f};
  return {result.x, result.y};
}

void SpriteRenderer::beginFrame(Camera* camera) {
  m_projection = camera->projectionMatrix();
  m_view = camera->viewMatrix();
}

void SpriteRenderer::renderSprite(Sprite* sprite, const ca::Vec2& position, F32 scale) {
  ca::Mat4 model = ca::translationMatrix(ca::Vec3{position, 0.0f}) * ca::scaleMatrix(scale);
  ca::Mat4 mvp = m_projection * m_view * model;

  ca::UniformBuffer uniforms;
  uniforms.set(m_quad.transformUniformId, mvp);
  uniforms.set(m_quad.textureTransformUniformId, ca::Mat4::identity);
  m_renderer->draw(ca::DrawType::Triangles, 6, m_quad.programId, m_quad.vertexBufferId,
                   m_quad.indexBufferId, sprite->textureId, uniforms);
}
