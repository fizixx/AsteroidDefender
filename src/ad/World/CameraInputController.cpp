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

    delta *= m_mouseSensitivity * -1.0f;

    m_camera->rotateRelative(delta.x, delta.y);

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
      m_moveDirection.y -= 1.0;
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

    default:
      break;
  }
}

void CameraInputController::tick(F32 delta) {
  // NOTE: This recalculates the view matrix of the camera twice!!!!!!!!

  auto forwardMovement = m_camera->forward() * m_moveDirection.y;
  auto rightMovement = m_camera->right() * m_moveDirection.x;

  m_camera->moveRelative((forwardMovement + rightMovement) * delta);
}
