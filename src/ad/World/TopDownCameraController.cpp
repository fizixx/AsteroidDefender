#include "ad/World/TopDownCameraController.h"

constexpr F32 kMouseSensitivity = 0.005f;
constexpr F32 kKeyboardSensitivity = 0.5f;

TopDownCameraController::TopDownCameraController(Camera* camera, F32 height)
  : m_camera{camera}, m_height{height} {}

void TopDownCameraController::tick(F32 delta) {
  m_camera->moveBy(m_movement.direction * delta);
}

void TopDownCameraController::onMouseMoved(const ca::MouseEvent& event) {
  if (m_movement.isMoving) {
    ca::Vec3 delta{
        static_cast<F32>(event.pos.x) - static_cast<F32>(m_movement.lastMousePosition.x),
        (static_cast<F32>(event.pos.y) - static_cast<F32>(m_movement.lastMousePosition.y)) * -1.0f,
        0.0f};

    m_movement.direction = delta * kMouseSensitivity;
  }
}

void TopDownCameraController::onMousePressed(const ca::MouseEvent& event) {
  if (event.button == ca::MouseEvent::Right) {
    m_movement.isMoving = true;
    m_movement.lastMousePosition = event.pos;
  }
}

void TopDownCameraController::onMouseReleased(const ca::MouseEvent& event) {
  if (event.button == ca::MouseEvent::Right) {
    m_movement.isMoving = false;
    m_movement.direction = ca::Vec3::zero;
  }
}

void TopDownCameraController::onMouseWheel(const ca::MouseWheelEvent& event) {
  m_camera->setFieldOfView(m_camera->fieldOfView() +
                           ca::degrees(-static_cast<F32>(event.wheelOffset.y)));
}

void TopDownCameraController::onKeyPressed(const ca::KeyEvent& event) {
  switch (event.key) {
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

void TopDownCameraController::onKeyReleased(const ca::KeyEvent& event) {
  switch (event.key) {
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
