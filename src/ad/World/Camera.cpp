#include "ad/World/Camera.h"

#include "canvas/Math/Intersection.h"
#include "canvas/Math/Transform.h"
#include "nucleus/Macros.h"

namespace {

ca::Mat4 createCameraMatrix(const ca::Vec3& position, const ca::Vec3& direction,
                            const ca::Vec3& worldUp) {
  // We have the direction the camera is facing and the up vector, so we can calculate a right and
  // up vectors:
  ca::Vec3 cameraForward = ca::normalize(direction);
  ca::Vec3 cameraRight = ca::normalize(ca::crossProduct(cameraForward, worldUp));
  ca::Vec3 cameraUp = ca::crossProduct(cameraRight, cameraForward);

  ca::Mat4 basis{cameraRight, cameraUp, -cameraForward, position};

  // TODO: We can do a faster inverse here, because we know the values we put into the matrix.
  return ca::inverse(basis);
}

}  // namespace

Camera::Camera(const ca::Vec3& worldUp)
  : m_size{1.0f, 1.0f},
    m_worldUp{worldUp},
    m_projection{ca::Mat4::identity},
    m_view{ca::Mat4::identity} {}

void Camera::resize(const ca::Size& size) {
  m_size = {static_cast<F32>(size.width), static_cast<F32>(size.height)};
}

void Camera::moveTo(const ca::Vec3& position) {
  m_position = position;
}

void Camera::moveRelative(const ca::Vec3& relativePosition) {
  moveTo(m_position + relativePosition);
}

void Camera::reorient(const ca::Vec3& direction) {
  m_direction = ca::normalize(direction);
}

ca::Ray Camera::createRay() const {
  return {m_position, m_direction};
}

ca::Ray Camera::createRayForMouse(const ca::Vec2& mousePosition) {
  ca::Mat4 cameraInverse = ca::inverse(m_view * m_projection);

  auto positionInWorldSpace =
      cameraInverse * ca::Vec4{mousePosition.x, mousePosition.y, 0.0f, 1.0f};

#if 0
  LOG(Info) << "m_current.position: " << m_current.position
            << ", positionInWorldSpace: " << positionInWorldSpace;
#endif

  return {m_position,
          ca::Vec3{positionInWorldSpace.x, positionInWorldSpace.y, positionInWorldSpace.z}};
}

void Camera::tick(F32 UNUSED(delta)) {
  F32 aspectRatio = m_size.x / m_size.y;
  m_projection = ca::perspectiveProjection(45.0f, aspectRatio, 0.1f, 1000.0f);

  // NOTE: The direction should point in the reverse direction as where the camera is pointing.
  m_view = createCameraMatrix(m_position, m_direction, m_worldUp);
}
