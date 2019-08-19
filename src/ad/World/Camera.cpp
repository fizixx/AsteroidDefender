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

void Camera::move(const ca::Vec3& offset) {
  moveTo(m_position + offset);
}

void Camera::yaw(ca::Angle angle) {
  rotate(m_orientation * ca::Vec3::up, angle);
}

void Camera::pitch(ca::Angle angle) {
  rotate(m_orientation * ca::Vec3::right, angle);

  updateViewMatrix();
}

void Camera::roll(ca::Angle angle) {
  rotate(m_orientation * ca::Vec3::forward, angle);

  updateViewMatrix();
}

void Camera::rotate(const ca::Vec3& axis, ca::Angle angle) {
  rotate(ca::fromAxisAngle(axis, angle));
}

void Camera::rotate(const ca::Quaternion& orientation) {
  m_orientation = normalize(orientation * m_orientation);

  updateViewMatrix();
}

ca::Ray Camera::createRay() const {
  return {m_position, m_forward};
}

ca::Ray Camera::createRayForMouse(const ca::Vec2& mousePosition) {
#if 0
  ca::Mat4 cameraInverse = ca::inverse(m_projection);

  auto positionInWorldSpace =
      cameraInverse * ca::Vec4{mousePosition.x, mousePosition.y, 1.0f, 1.0f};

  positionInWorldSpace = ca::inverse(m_view) * positionInWorldSpace;

#if 0
  LOG(Info) << "mousePosition: " << mousePosition << ", m_position: " << m_position
            << ", positionInWorldSpace: " << positionInWorldSpace;
#endif

  return {m_position,
          ca::Vec3{positionInWorldSpace.x, positionInWorldSpace.y, positionInWorldSpace.z}};
#endif  // 0

  ca::Mat4 inverse = ca::inverse(m_projection * m_view);

  ca::Vec3 nearPoint{mousePosition.x, mousePosition.y, -1.0f};
  ca::Vec3 midPoint{mousePosition.x, mousePosition.y, 0.0f};

  ca::Vec4 rayOrigin = inverse * ca::Vec4{nearPoint, 1.0f};
  ca::Vec4 rayTarget = inverse * ca::Vec4{midPoint, 1.0f};

  ca::Vec3 rayDirection = ca::normalize(rayTarget.xyz() - rayOrigin.xyz());

  return {rayOrigin.xyz(), rayDirection};
}

void Camera::updateProjectionMatrix() {
  const F32 aspectRatio = m_size.x / m_size.y;
  m_projection = ca::perspectiveProjection(45.0f, aspectRatio, 0.1f, 1000.0f);
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
  ca::Mat4 viewMatrix{rotationMatrix};
  //ca::Mat4 viewMatrix = ca::Mat4::identity;
  viewMatrix.col[3] = ca::Vec4{m_position, 1.0f};

  m_view = ca::inverse(viewMatrix);
}
