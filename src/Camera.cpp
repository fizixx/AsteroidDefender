#include "Camera.h"

#include "canvas/Math/Transform.h"

ca::Mat4 Camera::calculateViewMatrix() {
  return ca::translationMatrix({position.x, position.y, -zoom});
}
