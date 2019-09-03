#include "ad/World/CameraController.h"
#include "ad/World/Camera.h"

CameraController::CameraController(Camera* camera, F32 mouseSensitivity)
  : m_camera(camera), m_mouseSensitivity{mouseSensitivity} {}

void CameraController::onMouseMoved(const ca::MouseEvent& event) {
  if (m_mouseIsDown) {
    ca::Vec2 delta{
        static_cast<F32>(event.pos.x - m_lastMousePosition.x),
        static_cast<F32>(event.pos.y - m_lastMousePosition.y),
    };

    m_yaw += -delta.x * m_mouseSensitivity;
    m_pitch += -delta.y * m_mouseSensitivity;

    m_camera->rotateTo(ca::Quaternion::fromEulerAngles(ca::degrees(m_pitch), ca::degrees(m_yaw),
                                                       ca::degrees(0.0f)));

    m_lastMousePosition = event.pos;
  }
}

void CameraController::onMousePressed(const ca::MouseEvent& event) {
  if (event.button == ca::MouseEvent::Left) {
    m_mouseIsDown = true;
    m_lastMousePosition = event.pos;
  }
}

void CameraController::onMouseReleased(const ca::MouseEvent& event) {
  if (event.button == ca::MouseEvent::Left) {
    m_mouseIsDown = false;
  }
}

void CameraController::onKeyPressed(const ca::KeyEvent& event) {
  switch (event.key) {
    case ca::Key::A:
      m_moveDirection.x -= 1.0f;
      break;

    case ca::Key::D:
      m_moveDirection.x += 1.0f;
      break;

    case ca::Key::W:
      m_moveDirection.y += 1.0f;
      break;

    case ca::Key::S:
      m_moveDirection.y -= 1.0f;
      break;

    case ca::Key::Q:
      m_moveDirection.z += 1.0f;
      break;

    case ca::Key::Z:
      m_moveDirection.z -= 1.0f;
      break;

    default:
      break;
  }
}

void CameraController::onKeyReleased(const ca::KeyEvent& event) {
  switch (event.key) {
    case ca::Key::A:
      m_moveDirection.x += 1.0f;
      break;

    case ca::Key::D:
      m_moveDirection.x -= 1.0f;
      break;

    case ca::Key::W:
      m_moveDirection.y -= 1.0f;
      break;

    case ca::Key::S:
      m_moveDirection.y += 1.0;
      break;

    case ca::Key::Q:
      m_moveDirection.z -= 1.0f;
      break;

    case ca::Key::Z:
      m_moveDirection.z += 1.0f;
      break;

    default:
      break;
  }
}

void CameraController::tick(F32 delta) {
  constexpr F32 kMovementSpeed = 0.1f;

  auto forwardMovement = m_camera->forward() * m_moveDirection.y;
  auto rightMovement = m_camera->right() * m_moveDirection.x;
  auto upMovement = m_camera->up() * m_moveDirection.z;

  m_camera->moveBy((forwardMovement + rightMovement + upMovement) * kMovementSpeed * delta);
}