#ifndef AD_CAMERA_INPUT_CONTROLLER_H_
#define AD_CAMERA_INPUT_CONTROLLER_H_

#include "canvas/Math/Vec3.h"
#include "canvas/Windows/Event.h"
#include "nucleus/Macros.h"

class Camera;

class CameraInputController {
public:
  explicit CameraInputController(Camera* camera, F32 mouseSensitivity = 1.0f);

  void onMouseMoved(const ca::MouseEvent& event);
  void onMousePressed(const ca::MouseEvent& event);
  void onMouseReleased(const ca::MouseEvent& event);

  void onKeyPressed(const ca::KeyEvent& event);
  void onKeyReleased(const ca::KeyEvent& event);

  void tick(F32 delta);

private:
  DELETE_COPY_AND_MOVE(CameraInputController);

  // The camera we are controlling.
  Camera* m_camera;

  // The speed at which the camera on mouse movement.
  F32 m_mouseSensitivity;

  // Keep track of the current pitch and yaw values that we pass to the camera.
  F32 m_yaw = 0.0f;
  F32 m_pitch = 0.0f;

  ca::Vec3 m_moveDirection{0.0f, 0.0f, 0.0f};

  bool m_mouseIsDown = false;
  ca::Pos m_lastMousePosition{0, 0};
};

#endif  // AD_CAMERA_INPUT_CONTROLLER_H_
