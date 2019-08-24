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

Camera::Camera(ca::Angle fieldOfView, const ca::Vec3& worldUp)
  : m_fieldOfView{fieldOfView}, m_worldUp{ca::normalize(worldUp)} {
  updateProjectionMatrix();
  updateViewMatrix();
}

void Camera::resize(const ca::Size& size) {
  resize(ca::Vec2{static_cast<F32>(size.width), static_cast<F32>(size.height)});
}

void Camera::resize(const ca::Vec2& size) {
  m_size = size;

  invalidateProjection();
}

void Camera::setNearPlane(F32 nearPlane) {
  m_nearPlane = nearPlane;
  invalidateProjection();
}

void Camera::setFarPlane(F32 farPlane) {
  m_farPlane = farPlane;
  invalidateProjection();
}

void Camera::setFieldOfView(ca::Angle fieldOfView) {
  m_fieldOfView = fieldOfView;
  invalidateProjection();
}

void Camera::moveTo(const ca::Vec3& position) {
  m_position = position;

  invalidateView();
}

void Camera::moveBy(const ca::Vec3& offset) {
  moveTo(m_position + offset);
}

void Camera::rotateTo(const ca::Quaternion& orientation) {
  m_orientation = orientation;

  invalidateView();
}

void Camera::rotateBy(const ca::Vec3& axis, ca::Angle angle) {
  rotateBy(ca::fromAxisAngle(axis, angle));
}

void Camera::rotateBy(const ca::Quaternion& orientation) {
  m_orientation = orientation * m_orientation;

  invalidateView();
}

ca::Ray Camera::createRay() const {
  return {m_position, m_forwardVector};
}

ca::Ray Camera::createRayForMouse(const ca::Vec2& mousePosition) {
  updateProjectionMatrix();
  updateViewMatrix();

  ca::Mat4 inverseVP = ca::inverse(m_projectionMatrix * m_viewMatrix);

  F32 nx = (2.0f * mousePosition.x / m_size.x) - 1.0f;
  F32 ny = 1.0f - (2.0f * mousePosition.y / m_size.y);

  ca::Vec4 rayOrigin = inverseVP * ca::Vec4{nx, ny, -1.f, 1.0f};
  ca::Vec4 rayTarget = inverseVP * ca::Vec4{nx, ny, 0.0f, 1.0f};

  // LOG(Info) << "rayOrigin = " << rayOrigin << ", rayTarget = " << rayTarget;

  ca::Vec4 rayDirection = rayTarget - rayOrigin;

  return ca::Ray{rayOrigin.xyz(), ca::normalize(rayDirection.xyz())};
}

void Camera::updateProjectionMatrix(ca::Mat4* projectionMatrix) {
  if (m_dirtyFlags & static_cast<U32>(DirtyFlag::Projection)) {
    m_dirtyFlags &= ~static_cast<U32>(DirtyFlag::Projection);
    updateProjectionMatrix();
  }

  *projectionMatrix = m_projectionMatrix;
}

void Camera::updateViewMatrix(ca::Mat4* viewMatrix) {
  if (m_dirtyFlags & static_cast<U32>(DirtyFlag::View)) {
    m_dirtyFlags &= ~static_cast<U32>(DirtyFlag::View);
    updateViewMatrix();
  }

  *viewMatrix = m_viewMatrix;
}

void Camera::invalidateProjection() {
  m_dirtyFlags |= static_cast<U32>(DirtyFlag::Projection);
}

void Camera::invalidateView() {
  m_dirtyFlags |= static_cast<U32>(DirtyFlag::View);
}

void Camera::updateProjectionMatrix() {
  const F32 aspectRatio = m_size.x / m_size.y;
  m_projectionMatrix =
      ca::perspectiveProjection(m_fieldOfView, aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::updateViewMatrix() {
  m_viewMatrix = ca::createViewMatrix(m_position, m_orientation);

  ca::Mat4 t = ca::transpose(m_viewMatrix);

  m_rightVector = t.col[0].xyz();
  m_upVector = t.col[1].xyz();
  m_forwardVector = -t.col[2].xyz();
}
