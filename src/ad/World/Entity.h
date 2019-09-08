#ifndef AD_WORLD_ENTITY_H_
#define AD_WORLD_ENTITY_H_

#include "canvas/Math/Common.h"
#include "canvas/Math/Vec2.h"

struct Model;

const MemSize kInvalidEntityId = std::numeric_limits<MemSize>::max();

struct EntityId {
  MemSize id = kInvalidEntityId;

  bool isValid() {
    return id != kInvalidEntityId;
  }
};

struct Entity {
  ca::Vec2 position = ca::Vec2::zero;
  Model* model = nullptr;

  struct {
    ca::Angle direction = ca::degrees(0.0f);
    F32 speed = 0.0f;

    F32 distanceTravelled = 0.0f;
  } movement;
};

#endif  // AD_WORLD_ENTITY_H_
