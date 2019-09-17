#ifndef AD_WORLD_CAMERA_CONTROLLER_H_
#define AD_WORLD_CAMERA_CONTROLLER_H_

#include "ad/World/Camera.h"
#include "canvas/Windows/Event.h"

class CameraController {
public:
  explicit CameraController(Camera* camera) : m_camera{camera} {}

  Camera* camera() const {
    return m_camera;
  }

  virtual void onMouseMoved(const ca::Vec2& position) = 0;
  virtual void onMousePressed(ca::MouseEvent::Button button, const ca::Vec2& position) = 0;
  virtual void onMouseReleased(ca::MouseEvent::Button button, const ca::Vec2& position) = 0;
  virtual void onMouseWheel(const ca::Vec2& offset) = 0;

  virtual void onKeyPressed(ca::Key key) = 0;
  virtual void onKeyReleased(ca::Key key) = 0;

  virtual void tick(F32 delta) = 0;

protected:
  Camera* m_camera;

private:
  DELETE_COPY_AND_MOVE(CameraController);
};

#endif  // AD_WORLD_CAMERA_CONTROLLER_H_
