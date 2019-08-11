#ifndef CAMERA_H_
#define CAMERA_H_

#include "canvas/Math/Mat4.h"
#include "canvas/Math/Vec2.h"
#include "canvas/Utils/Pos.h"
#include "canvas/Utils/Size.h"

class Camera {
public:
  Camera();

  void resize(const ca::Size& size);

  const ca::Vec2& position() const {
    return m_current.position;
  }

  void moveTo(const ca::Vec2& position);
  void moveRelative(const ca::Vec2& position);

  F32 zoom() const {
    return m_current.zoom;
  }

  void zoomTo(F32 zoom);
  void zoomRelative(F32 relativeZoom);

  const ca::Mat4& projectionMatrix() const {
    return m_projection;
  }

  const ca::Mat4& viewMatrix() const {
    return m_view;
  }

  ca::Vec2 calculateCursorPositionInWorld(const ca::Vec2& screenPosition);

  void tick(F32 delta);

private:
  // The size of the viewport we're rendering to.
  ca::Vec2 m_size;

  struct {
    ca::Vec2 position{0.0f, 0.0f};
    F32 zoom = 100.0f;
  } m_current, m_target;

  ca::Mat4 m_projection;
  ca::Mat4 m_view;
};

#endif  // CAMERA_H_
