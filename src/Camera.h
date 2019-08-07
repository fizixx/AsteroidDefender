#ifndef CAMERA_H_
#define CAMERA_H_

#include "canvas/Math/Mat4.h"
#include "canvas/Math/Vec2.h"

struct Camera {
  ca::Vec2 position{0.0f, 0.0f};

  ca::Mat4 calculateViewMatrix();
};

#endif  // CAMERA_H_
