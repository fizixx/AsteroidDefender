#include "ad/World/Camera.h"

#include "canvas/Math/Transform.h"
#include "nucleus/Logging.h"
#include "nucleus/Macros.h"

namespace {

struct Plane {
  ca::Vec3 position;
  ca::Vec3 normal;
};

inline ca::Vec3 intersect(const Plane& plane, ca::Vec3 linePos1, ca::Vec3 linePos2) {
  ca::Vec3 v = linePos2 - linePos1;
  ca::Vec3 w = plane.position - linePos1;

  F32 k = dotProduct(w, plane.normal) / dotProduct(v, plane.normal);

  ca::Vec3 intersection = linePos1 + v * k;

  // B32 didIntersect = k >= 0.0f && k <= 1.0f;

  return intersection;
}

ca::Mat4 createCameraMatrix(const ca::Vec3& position, const ca::Vec3& direction,
                            const ca::Vec3& up) {
  // We have the direction the camera is facing and the up vector, so we can calculate a right and
  // up vector for the camera:
  ca::Vec3 cameraRight = ca::normalize(ca::crossProduct(direction, up));
  ca::Vec3 cameraUp = ca::crossProduct(cameraRight, direction);

  ca::Mat4 m{cameraRight, cameraUp, -direction, position};

  // TODO: We can do a faster inverse here, because we know the values we put into the matrix.
  return ca::inverse(m);
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const ca::Vec4& value) {
  os << "{" << value.x << ", " << value.y << ", " << value.z << ", " << value.w << "}";
  return os;
}

Camera::Camera()
  : m_size{0.0f, 0.0f}, m_projection{ca::Mat4::identity}, m_view{ca::Mat4::identity} {}

void Camera::resize(const ca::Size& size) {
  m_size = {static_cast<F32>(size.width), static_cast<F32>(size.height)};
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

ca::Vec2 Camera::calculateCursorPositionInWorld(const ca::Vec2& screenPosition) {
  ca::Mat4 m = ca::inverse(m_projection);

  F32 mouseX = (2.0f * screenPosition.x) / m_size.x - 1.0f;
  F32 mouseY = (2.0f * screenPosition.y) / m_size.y - 1.0f;
  F32 mouseZ = 1.0f;

  ca::Vec4 pc = ca::Vec4{mouseX, mouseY, mouseZ, 1.0f};

  ca::Vec4 p = m * pc;
  // p /= p.w;

  Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
  auto intersectPoint =
      intersect(plane, ca::Vec3{m_current.position, m_current.zoom} + ca::Vec3{p.x, p.y, p.z},
                ca::Vec3{m_current.position, m_current.zoom});

  LOG(Info) << "pc = " << pc << ", p = " << p;

  return {intersectPoint.x, intersectPoint.y};
}

void Camera::tick(F32 UNUSED(delta)) {
  m_current.position.x += (m_target.position.x - m_current.position.x) * 0.1f;
  m_current.position.y += (m_target.position.y - m_current.position.y) * 0.1f;
  m_current.zoom += (m_target.zoom - m_current.zoom) * 0.1f;

  F32 aspectRatio = m_size.x / m_size.y;
  m_projection = ca::perspectiveProjection(60.0f, aspectRatio, 0.1f, 1000.0f);

  m_view = createCameraMatrix({m_current.position.x, m_current.position.y, -m_current.zoom},
                              {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f});

  ca::Vec4 inPosition{1.0f, 2.0f, 3.0f, 1.0f};
  ca::Vec4 result = (m_projection * m_view) * inPosition;

  ca::Mat4 inv = ca::inverse(m_projection * m_view);
  auto outPosition = inv * result;

  LOG(Info) << "in = " << inPosition << ", out = " << outPosition;
}
