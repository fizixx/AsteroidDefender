#ifndef SPRITE_H_
#define SPRITE_H_

#include "canvas/Math/Mat4.h"
#include "canvas/Renderer/Types.h"
#include "canvas/Utils/Size.h"

struct Sprite {
  ca::Size size;
  ca::TextureId textureId;
};

#endif  // SPRITE_H_
