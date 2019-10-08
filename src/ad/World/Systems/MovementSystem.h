#ifndef AD_WORLD_SYSTEMS_MOVEMENT_SYSTEM_H_
#define AD_WORLD_SYSTEMS_MOVEMENT_SYSTEM_H_

#include "ad/World/Entity.h"

struct MovementSystem {
  auto tick(EntityList& entities, F32 delta) -> void {
    for (auto& entity : entities) {
      if (entity.movement.speed > 0.0f) {
        ca::Vec2 d{ca::cosine(entity.movement.direction), ca::sine(entity.movement.direction)};
        F32 distance = entity.movement.speed * 0.01f * delta;
        entity.position += d * distance;

        entity.movement.distanceTravelled += distance;

        if (entity.movement.distanceTravelled > 5.0f) {
          entity.movement.direction = ca::degrees((F32)(std::rand() % 360));
          entity.movement.distanceTravelled = 0.0f;
        }
      }
    }
  }
};

#endif  // AD_WORLD_SYSTEMS_MOVEMENT_SYSTEM_H_
