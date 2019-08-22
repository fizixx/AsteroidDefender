#include "ad/World/CameraInputController.h"
#include "ad/World/Camera.h"
#include "nucleus/Logging.h"

CameraInputController::CameraInputController(Camera* camera, F32 mouseSensitivity)
  : m_camera(camera), m_mouseSensitivity{mouseSensitivity} {}

void CameraInputController::onMouseMoved(const ca::MouseEvent& event) {
  if (m_mouseIsDown) {
    ca::Vec2 delta{
        static_cast<F32>(event.pos.x - m_lastMousePosition.x),
        static_cast<F32>(event.pos.y - m_lastMousePosition.y),
    };

    m_yaw += -delta.x * m_mouseSensitivity;
    m_pitch += -delta.y * m_mouseSensitivity;

    m_camera->setRotation(ca::Quaternion::fromEulerAngles(ca::degrees(m_pitch), ca::degrees(m_yaw),
                                                          ca::degrees(0.0f)));

    m_lastMousePosition = event.pos;
  }
}

void CameraInputController::onMousePressed(const ca::MouseEvent& event) {
  if (event.button == ca::MouseEvent::Left) {
    m_mouseIsDown = true;
    m_lastMousePosition = event.pos;
  }
}

void CameraInputController::onMouseReleased(const ca::MouseEvent& event) {
  if (event.button == ca::MouseEvent::Left) {
    m_mouseIsDown = false;
  }
}

void CameraInputController::onKeyPressed(const ca::KeyEvent& event) {
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

void CameraInputController::onKeyReleased(const ca::KeyEvent& event) {
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

void CameraInputController::tick(F32 delta) {
  constexpr F32 kMovementSpeed = 3.0f;

  auto forwardMovement = m_camera->forward() * m_moveDirection.y;
  auto rightMovement = m_camera->right() * m_moveDirection.x;
  auto upMovement = m_camera->up() * m_moveDirection.z;

  m_camera->move((forwardMovement + rightMovement + upMovement) * kMovementSpeed * delta);
}
