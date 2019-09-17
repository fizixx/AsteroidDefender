#include "ad/World/OrbitCameraController.h"

OrbitCameraController::OrbitCameraController(Camera* camera, const ca::Vec3& origin)
  : CameraController{camera}, m_origin{origin} {}

void OrbitCameraController::onMouseMoved(const ca::Vec2& position) {}

void OrbitCameraController::onMousePressed(ca::MouseEvent::Button button,
                                           const ca::Vec2& position) {}

void OrbitCameraController::onMouseReleased(ca::MouseEvent::Button button,
                                            const ca::Vec2& position) {}

void OrbitCameraController::onMouseWheel(const ca::Vec2& offset) {}

void OrbitCameraController::onKeyPressed(ca::Key key) {}

void OrbitCameraController::onKeyReleased(ca::Key key) {}

void OrbitCameraController::tick(F32 delta) {
  LOG(Info) << "moving camera";
  m_camera->moveTo(ca::Vec3{0.0f, 0.0f, 100.0f});
}
