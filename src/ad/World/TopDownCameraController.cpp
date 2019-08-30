#include "ad/World/TopDownCameraController.h"
#include "canvas/Math/Intersection.h"

constexpr F32 kMouseSensitivity = 0.005f;
constexpr F32 kKeyboardSensitivity = 0.5f;

TopDownCameraController::TopDownCameraController(Camera* camera, const ca::Plane& worldPlane,
                                                 F32 height)
  : m_camera{camera}, m_worldPlane{worldPlane}, m_height{height} {}

void TopDownCameraController::tick(F32 delta) {
  m_camera->moveBy(m_movement.direction * delta);

  // TODO: fix
  m_height = 1.0f;
}

void TopDownCameraController::onMouseMoved(const ca::Vec2& position) {
  if (m_movement.isMoving) {
    ca::Vec3 newPositionInWorld = getMousePositionInWorld(position);
    ca::Vec3 worldDelta = m_movement.lastMousePositionInWorld - newPositionInWorld;

    m_camera->moveBy(worldDelta);
  }
}

void TopDownCameraController::onMousePressed(ca::MouseEvent::Button button,
                                             const ca::Vec2& position) {
  if (button == ca::MouseEvent::Right) {
    m_movement.isMoving = true;
    m_movement.lastMousePositionInWorld = getMousePositionInWorld(position);
  }
}

void TopDownCameraController::onMouseReleased(ca::MouseEvent::Button button,
                                              const ca::Vec2& position) {
  if (button == ca::MouseEvent::Right) {
    m_movement.isMoving = false;
  }
}

void TopDownCameraController::onMouseWheel(const ca::Vec2& offset) {
  m_camera->setFieldOfView(m_camera->fieldOfView() + ca::degrees(-offset.y));
}

void TopDownCameraController::onKeyPressed(ca::Key key) {
  switch (key) {
    case ca::Key::A:
      m_movement.direction -= ca::Vec3::right * kKeyboardSensitivity;
      break;

    case ca::Key::D:
      m_movement.direction += ca::Vec3::right * kKeyboardSensitivity;
      break;

    case ca::Key::W:
      m_movement.direction += ca::Vec3::up * kKeyboardSensitivity;
      break;

    case ca::Key::S:
      m_movement.direction -= ca::Vec3::up * kKeyboardSensitivity;
      break;

    default:
      break;
  }
}

void TopDownCameraController::onKeyReleased(ca::Key key) {
  switch (key) {
    case ca::A:
      m_movement.direction += ca::Vec3::right * kKeyboardSensitivity;
      break;

    case ca::Key::D:
      m_movement.direction -= ca::Vec3::right * kKeyboardSensitivity;
      break;

    case ca::Key::W:
      m_movement.direction -= ca::Vec3::up * kKeyboardSensitivity;
      break;

    case ca::Key::S:
      m_movement.direction += ca::Vec3::up * kKeyboardSensitivity;
      break;

    default:
      break;
  }
}

ca::Vec3 TopDownCameraController::getMousePositionInWorld(const ca::Vec2& mousePosition) const {
  auto ray = m_camera->createRayForMouse(mousePosition);
  auto result = ca::intersection(m_worldPlane, ray);
  return result.position;
}
