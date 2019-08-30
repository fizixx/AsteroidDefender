#ifndef AD_WORLD_TOP_DOWN_CAMERA_CONTROLLER_H_
#define AD_WORLD_TOP_DOWN_CAMERA_CONTROLLER_H_

#include "ad/World/Camera.h"
#include "canvas/Math/Plane.h"
#include "canvas/Windows/Event.h"

class TopDownCameraController {
public:
  TopDownCameraController(Camera* camera, const ca::Plane& worldPlane, F32 height);

  void tick(F32 delta);

  void onMouseMoved(const ca::Vec2& position);
  void onMousePressed(ca::MouseEvent::Button button, const ca::Vec2& position);
  void onMouseReleased(ca::MouseEvent::Button button, const ca::Vec2& position);
  void onMouseWheel(const ca::Vec2& offset);

  void onKeyPressed(ca::Key key);
  void onKeyReleased(ca::Key key);

private:
  ca::Vec3 getMousePositionInWorld(const ca::Vec2& mousePosition) const;

  Camera* m_camera;
  ca::Size m_viewportSize;
  ca::Plane m_worldPlane;
  F32 m_height;

  struct {
    bool isMoving = false;
    ca::Vec3 lastMousePositionInWorld{ca::Vec3::zero};
    ca::Vec3 direction{ca::Vec3::zero};
  } m_movement;
};

#endif  // AD_WORLD_TOP_DOWN_CAMERA_CONTROLLER_H_
