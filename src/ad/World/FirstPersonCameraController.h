#ifndef AD_FIRST_PERSON_CAMERA_INPUT_CONTROLLER_H_
#define AD_FIRST_PERSON_CAMERA_INPUT_CONTROLLER_H_

#include "ad/World/CameraController.h"
#include "canvas/Math/Vec3.h"

class Camera;

class FirstPersonCameraController : public CameraController {
public:
  explicit FirstPersonCameraController(Camera* camera, F32 mouseSensitivity = 1.0f);

  // Override: CameraController
  void onMouseMoved(const ca::Vec2& position) override;
  void onMousePressed(ca::MouseEvent::Button button, const ca::Vec2& position) override;
  void onMouseReleased(ca::MouseEvent::Button button, const ca::Vec2& position) override;
  void onMouseWheel(const ca::Vec2& offset) override;
  void onKeyPressed(ca::Key key) override;
  void onKeyReleased(ca::Key key) override;
  void tick(F32 delta) override;

private:
  DELETE_COPY_AND_MOVE(FirstPersonCameraController);

  // The speed at which the camera on mouse movement.
  F32 m_mouseSensitivity;

  // Keep track of the current pitch and yaw values that we pass to the camera.
  F32 m_yaw = 0.0f;
  F32 m_pitch = 0.0f;

  ca::Vec3 m_moveDirection{0.0f, 0.0f, 0.0f};

  bool m_mouseIsDown = false;
  ca::Vec2 m_lastMousePosition = ca::Vec2::zero;
};

#endif  // AD_FIRST_PERSON_CAMERA_INPUT_CONTROLLER_H_
