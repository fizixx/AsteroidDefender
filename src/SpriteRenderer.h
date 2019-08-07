#ifndef SPRITE_RENDERER_H_
#define SPRITE_RENDERER_H_

#include "Sprite.h"

#include "Camera.h"
#include "canvas/Renderer/Renderer.h"
#include "nucleus/Macros.h"

class SpriteRenderer {
public:
  SpriteRenderer() = default;
  ~SpriteRenderer() = default;

  bool initialize(ca::Renderer* renderer);

  void resize(const ca::Size& size);

  ca::Vec2 convertDeviceCoordinatesToWorldCoordinates(ca::Pos pos);

  void tick(F32 delta);

  void beginFrame(Camera* camera);
  void renderSprite(Sprite* sprite, const ca::Vec2& position, F32 scale = 1.0f);


private:
  DELETE_COPY_AND_MOVE(SpriteRenderer);

  ca::Renderer* m_renderer = nullptr;

  F32 m_zoom = 0.1f;

  ca::Mat4 m_projection = ca::Mat4::identity;

  // The view matrix gets updated on every frame through `beginFrame(...)`.
  ca::Mat4 m_view = ca::Mat4::identity;

  struct {
    ca::VertexBufferId vertexBufferId;
    ca::IndexBufferId indexBufferId;
    ca::ProgramId programId;
    ca::UniformId transformUniformId;
    ca::UniformId textureTransformUniformId;
  } m_quad;
};

#endif  // SPRITE_H_
