#ifndef CAMERA_H_
#define CAMERA_H_

#include "canvas/Math/Mat4.h"
#include "canvas/Math/Ray.h"
#include "canvas/Math/Vec2.h"
#include "canvas/Utils/Pos.h"
#include "canvas/Utils/Size.h"

class Camera {
public:
  explicit Camera(const ca::Vec3& worldUp = {0.0f, 1.0f, 0.0f});

  void resize(const ca::Size& size);
  void resize(const ca::Vec2& size);

  const ca::Vec3& position() const {
    return m_position;
  }

  void moveTo(const ca::Vec3& position);
  void moveRelative(const ca::Vec3& position);

  const ca::Vec3& forward() const {
    return m_forward;
  }

  const ca::Vec3& right() const {
    return m_right;
  }

  const ca::Vec3& up() const {
    return m_up;
  }

  // Set the rotation of the camera in the horizontal and vertical orientations respectively
  void rotate(F32 yaw, F32 pitch);

  // Rotate the camera relatively to its current rotation.
  void rotateRelative(F32 relativeYaw, F32 relativePitch);

  // Create a ray that starts from the camera position and points towards the camera direction.
  ca::Ray createRay() const;

  // Create a ray that starts from the camera position and points towards a mouse pointer in clip
  // space.
  // NOTE: The mouse position should be in the range: [-1.0f..1.0f]
  ca::Ray createRayForMouse(const ca::Vec2& mousePosition);

  const ca::Mat4& projectionMatrix() const {
    return m_projection;
  }

  const ca::Mat4& viewMatrix() const {
    return m_view;
  }

private:
  void updateProjectionMatrix();
  void updateViewMatrix();

  // The size and of the viewport we're rendering to.
  ca::Vec2 m_size{1.0f, 1.0f};

  // The up vector for world space.
  ca::Vec3 m_worldUp;

  // The position of the camera in world space.
  ca::Vec3 m_position{0.0f, 0.0f, 0.0f};

  // Rotation angles.
  F32 m_yawDegrees = 0.0f;
  F32 m_pitchDegrees = 0.0f;

  // Basis vectors.
  ca::Vec3 m_forward{0.0f, 0.0f, -1.0f};
  ca::Vec3 m_right{1.0f, 0.0f, 0.0f};
  ca::Vec3 m_up{0.0f, 1.0f, 0.0f};

  ca::Mat4 m_projection = ca::Mat4::identity;
  ca::Mat4 m_view = ca::Mat4::identity;
};

#endif  // CAMERA_H_
