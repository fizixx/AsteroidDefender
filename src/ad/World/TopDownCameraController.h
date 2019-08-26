#ifndef AD_WORLD_TOP_DOWN_CAMERA_CONTROLLER_H_
#define AD_WORLD_TOP_DOWN_CAMERA_CONTROLLER_H_

#include "ad/World/Camera.h"
#include "canvas/Windows/Event.h"

class TopDownCameraController {
public:
  TopDownCameraController(Camera* camera, F32 height);

  void tick(F32 delta);

  void onMouseMoved(const ca::MouseEvent& event);
  void onMousePressed(const ca::MouseEvent& event);
  void onMouseReleased(const ca::MouseEvent& event);
  void onMouseWheel(const ca::MouseWheelEvent& event);

  void onKeyPressed(const ca::KeyEvent& event);
  void onKeyReleased(const ca::KeyEvent& event);

private:
  Camera* m_camera;
  F32 m_height;

  struct {
    bool isMoving = false;
    ca::Pos lastMousePosition;
    ca::Vec3 direction{0.0f, 0.0f, 0.0f};
  } m_movement;
};

#endif  // AD_WORLD_TOP_DOWN_CAMERA_CONTROLLER_H_
