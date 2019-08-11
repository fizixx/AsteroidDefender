#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "ad/Sprites/Sprite.h"
#include "canvas/Math/Vec2.h"

using EntityId = MemSize;

const EntityId kInvalidEntityId = std::numeric_limits<EntityId>::max();

struct Entity {
  ca::Vec2 position;
  F32 scale;

  Sprite* sprite;
};

#endif  // GEOMETRY_H_
