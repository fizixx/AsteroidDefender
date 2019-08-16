#include "ad/World/Camera.h"

#include "canvas/Math/Intersection.h"
#include "canvas/Math/Transform.h"
#include "nucleus/Logging.h"
#include "nucleus/Macros.h"

Camera::Camera(const ca::Vec3& worldUp) : m_worldUp{ca::normalize(worldUp)} {
  updateProjectionMatrix();
  updateViewMatrix();
}

void Camera::resize(const ca::Size& size) {
  resize(ca::Vec2{static_cast<F32>(size.width), static_cast<F32>(size.height)});
}

void Camera::resize(const ca::Vec2& size) {
  m_size = size;

  updateProjectionMatrix();
}

void Camera::moveTo(const ca::Vec3& position) {
  m_position = position;

  updateViewMatrix();
}

void Camera::moveRelative(const ca::Vec3& relativePosition) {
  moveTo(m_position + relativePosition);
}

void Camera::rotate(F32 yaw, F32 pitch) {
  m_yawDegrees = yaw;
  m_pitchDegrees = pitch;

  updateViewMatrix();
}

void Camera::rotateRelative(F32 relativeYaw, F32 relativePitch) {
  rotate(m_yawDegrees + relativeYaw, m_pitchDegrees + relativePitch);
}

ca::Ray Camera::createRay() const {
  return {m_position, m_forward};
}

ca::Ray Camera::createRayForMouse(const ca::Vec2& mousePosition) {
  ca::Mat4 cameraInverse = ca::inverse(m_projection);

  auto positionInWorldSpace =
      cameraInverse * ca::Vec4{mousePosition.x, mousePosition.y, 1.0f, 1.0f};

  // positionInWorldSpace = ca::inverse(m_view) * positionInWorldSpace;

#if 0
  LOG(Info) << "mousePosition: " << mousePosition << ", m_position: " << m_position
            << ", positionInWorldSpace: " << positionInWorldSpace;
#endif

  return {m_position,
          ca::Vec3{positionInWorldSpace.x, positionInWorldSpace.y, positionInWorldSpace.z}};
}

void Camera::updateProjectionMatrix() {
  const F32 aspectRatio = m_size.x / m_size.y;
  m_projection = ca::perspectiveProjection(45.0f, aspectRatio, 0.1f, 1000.0f);
}

void Camera::updateViewMatrix() {
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
}
