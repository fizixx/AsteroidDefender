#include "ad/World/Camera.h"

#include "canvas/Math/Transform.h"
#include "nucleus/Logging.h"

Camera::Camera() : m_projection{ca::Mat4::identity}, m_view{ca::Mat4::identity} {}

void Camera::resize(const ca::Size& size) {
  m_size = size;
}

void Camera::moveTo(const ca::Vec2& position) {
  m_target.position = position;
}

void Camera::moveRelative(const ca::Vec2& relativePosition) {
  moveTo(m_target.position + relativePosition);
}

void Camera::zoomTo(F32 zoom) {
  m_target.zoom = zoom;
  if (m_target.zoom < 1.0f) {
    m_target.zoom = 10.0f;
  }
  if (m_target.zoom > 200.0f) {
    m_target.zoom = 200.0f;
  }
}

void Camera::zoomRelative(F32 relativeZoom) {
  zoomTo(m_target.zoom + relativeZoom);
}

void Camera::tick(F32 delta) {
  m_current.position.x += (m_target.position.x - m_current.position.x) * 0.1f;
  m_current.position.y += (m_target.position.y - m_current.position.y) * 0.1f;
  m_current.zoom += (m_target.zoom - m_current.zoom) * 0.1f;

  F32 aspectRatio = static_cast<F32>(m_size.width) / static_cast<F32>(m_size.height);
  m_projection = ca::perspectiveProjection(60.0f, aspectRatio, 0.1f, 1000.0f);

  m_view = ca::translationMatrix({m_current.position.x, m_current.position.y, -m_current.zoom});
}
