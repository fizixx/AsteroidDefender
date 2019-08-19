#include "ad/World/Camera.h"

#include "canvas/Math/Intersection.h"
#include "canvas/Math/Transform.h"
#include "nucleus/Macros.h"

namespace {

enum class DirtyFlag : U32 {
  Projection = 0x0001u,
  View = 0x0002u,
};

}  // namespace

Camera::Camera(const ca::Vec3& worldUp) : m_worldUp{ca::normalize(worldUp)} {
  updateProjectionMatrix();
  updateViewMatrix();
}

void Camera::resize(const ca::Size& size) {
  resize(ca::Vec2{static_cast<F32>(size.width), static_cast<F32>(size.height)});
}

void Camera::resize(const ca::Vec2& size) {
  m_size = size;

  m_dirtyFlags |= static_cast<U32>(DirtyFlag::Projection);
}

void Camera::moveTo(const ca::Vec3& position) {
  m_position = position;

  m_dirtyFlags |= static_cast<U32>(DirtyFlag::View);
}

void Camera::move(const ca::Vec3& offset) {
  moveTo(m_position + offset);
}

void Camera::yaw(ca::Angle angle) {
  rotate(m_orientation * ca::Vec3::up, angle);
}

void Camera::pitch(ca::Angle angle) {
  rotate(m_orientation * ca::Vec3::right, angle);

  m_dirtyFlags |= static_cast<U32>(DirtyFlag::View);
}

void Camera::roll(ca::Angle angle) {
  rotate(m_orientation * ca::Vec3::forward, angle);

  m_dirtyFlags |= static_cast<U32>(DirtyFlag::View);
}

void Camera::rotate(const ca::Vec3& axis, ca::Angle angle) {
  rotate(ca::fromAxisAngle(axis, angle));
}

void Camera::rotate(const ca::Quaternion& orientation) {
  m_orientation = normalize(orientation * m_orientation);

  m_dirtyFlags |= static_cast<U32>(DirtyFlag::View);
}

ca::Ray Camera::createRay() const {
  return {m_position, m_forward};
}

ca::Ray Camera::createRayForMouse(const ca::Vec2& mousePosition) {
  ca::Mat4 inverseProjection = ca::inverse(m_projectionMatrix);
  ca::Mat4 inverseView = ca::inverse(m_viewMatrix);

  ca::Vec3 nearPoint{mousePosition.x, mousePosition.y, -1.0f};
  ca::Vec3 midPoint{mousePosition.x, mousePosition.y, 0.0f};

  ca::Vec4 rayOrigin = inverseView * inverseProjection * ca::Vec4{nearPoint, 1.0f};
  ca::Vec4 rayTarget = inverseView * inverseProjection * ca::Vec4{midPoint, 1.0f};

  ca::Vec3 rayDirection = ca::normalize(rayTarget.xyz() - rayOrigin.xyz());

  return {rayOrigin.xyz(), rayDirection};
}

void Camera::updateProjectionMatrix(ca::Mat4* projectionMatrix) {
  if (m_dirtyFlags & static_cast<U32>(DirtyFlag::Projection)) {
    updateProjectionMatrix();
  }

  *projectionMatrix = m_projectionMatrix;
}

void Camera::updateViewMatrix(ca::Mat4* viewMatrix) {
  if (m_dirtyFlags & static_cast<U32>(DirtyFlag::View)) {
    updateViewMatrix();
  }
  *viewMatrix = m_viewMatrix;
}

void Camera::updateProjectionMatrix() {
  const F32 aspectRatio = m_size.x / m_size.y;
  m_projectionMatrix = ca::perspectiveProjection(45.0f, aspectRatio, 0.1f, 1000.0f);
}

void Camera::updateViewMatrix() {
#if 0
  const F32 yawRadians = ca::degreesToRadians(m_yawDegrees);
  const F32 pitchRadians = ca::degreesToRadians(m_pitchDegrees);

  m_forward = ca::Vec3{
      ca::cosine(pitchRadians) * ca::sine(yawRadians),
      ca::sine(pitchRadians),
      ca::cosine(pitchRadians) * ca::cosine(yawRadians),
  };
  m_right = ca::crossProduct(m_forward, m_worldUp);
  m_up = ca::crossProduct(m_right, m_forward);

  m_view = ca::inverse(ca::Mat4{m_right, m_up, -m_forward, m_position});
#endif

  ca::Mat3 rotationMatrix = m_orientation.toRotationMatrix();
  ca::Mat4 viewMatrix{ca::transpose(rotationMatrix)};
  // ca::Mat4 viewMatrix = ca::Mat4::identity;
  viewMatrix.col[3] = ca::Vec4{-m_position, 1.0f};

  // m_view = ca::inverse(viewMatrix);
  m_viewMatrix = viewMatrix;
}
