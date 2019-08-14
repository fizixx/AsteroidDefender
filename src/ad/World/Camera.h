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

  const ca::Vec3& position() const {
    return m_current.position;
  }

  void moveTo(const ca::Vec3& position);
  void moveRelative(const ca::Vec3& position);

  const ca::Vec3& direction() const {
    return m_current.direction;
  }

  // Orient the camera to look at the target position.
  void reorient(const ca::Vec3& direction);

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

  void tick(F32 delta);

private:
  // The size and of the viewport we're rendering to.
  ca::Vec2 m_size;

  // The up vector for world space.
  ca::Vec3 m_worldUp;

  // The current and target attributes of the camera.  The current is always animated towards the
  // target on each `tick`.
  struct {
    // The position of the camera in world space.
    ca::Vec3 position{0.0f, 0.0f, 0.0f};

    // The direction the camera is pointing in world space.
    ca::Vec3 direction{0.0f, 0.0f, -1.0f};
  } m_current, m_target;

  ca::Mat4 m_projection;
  ca::Mat4 m_view;
};

#endif  // CAMERA_H_
