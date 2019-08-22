#ifndef CAMERA_H_
#define CAMERA_H_

#include "canvas/Math/Angle.h"
#include "canvas/Math/Mat4.h"
#include "canvas/Math/Quaternion.h"
#include "canvas/Math/Ray.h"
#include "canvas/Math/Vec2.h"
#include "canvas/Utils/Pos.h"
#include "canvas/Utils/Size.h"

class Camera {
public:
  explicit Camera(const ca::Vec3& worldUp = {0.0f, 1.0f, 0.0f});

  void resize(const ca::Size& size);
  void resize(const ca::Vec2& size);

  // Position

  const ca::Vec3& position() const {
    return m_position;
  }

  // Move the camera to the given `position`.
  void moveTo(const ca::Vec3& position);

  // Increase the camera's position by the given `offset`.
  void move(const ca::Vec3& offset);

  // Set the absolute orientation of the camera.
  void setRotation(const ca::Quaternion& orientation);

  // Increase the camera's orientation by the given `angle` around the given `axis`.
  void rotateBy(const ca::Vec3& axis, ca::Angle angle);

  // Increase the camera's orientation by the given `orientation`.
  void rotateBy(const ca::Quaternion& orientation);

  // Return the camera's current forward vector.
  ca::Vec3 forward() const {
    return m_viewMatrix.col[2].xyz();
  }

  // Return the camera's current right vector.
  ca::Vec3 right() const {
    return m_viewMatrix.col[0].xyz();
  }

  // Return the camera's current up vector.
  ca::Vec3 up() const {
    return m_viewMatrix.col[1].xyz();
  }

  // Rays

  // Create a ray that starts from the camera's current position and points towards the camera
  // forward direction.
  ca::Ray createRay() const;

  // Create a ray that starts from the camera's current position and points towards a mouse pointer
  // in clip space.
  // NOTE: The mouse position should be in the range: [-1.0f..1.0f]
  ca::Ray createRayForMouse(const ca::Vec2& mousePosition);

  void updateProjectionMatrix(ca::Mat4* projectionMatrix);
  void updateViewMatrix(ca::Mat4* viewMatrix);

private:
  void invalidateProjection();
  void invalidateView();

  void updateProjectionMatrix();
  void updateViewMatrix();

  // The size and of the viewport we're rendering to.
  ca::Vec2 m_size{1.0f, 1.0f};

  // The up vector for world space.
  ca::Vec3 m_worldUp;

  // The position of the camera in world space.
  ca::Vec3 m_position{0.0f, 0.0f, 0.0f};

  // Rotation angles.
  ca::Quaternion m_orientation;

  // Basis vectors.
  ca::Vec3 m_rightVector{1.0f, 0.0f, 0.0f};
  ca::Vec3 m_upVector{0.0f, 1.0f, 0.0f};
  ca::Vec3 m_forwardVector{0.0f, 0.0f, -1.0f};

  U32 m_dirtyFlags = 0u;

  ca::Mat4 m_projectionMatrix = ca::Mat4::identity;
  ca::Mat4 m_viewMatrix = ca::Mat4::identity;
};

#endif  // CAMERA_H_
