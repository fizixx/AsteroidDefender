#ifndef AD_WORLD_SYSTEMS_MOVEMENT_SYSTEM_H_
#define AD_WORLD_SYSTEMS_MOVEMENT_SYSTEM_H_

#include "ad/World/Entity.h"

struct MovementSystem {
  auto tick(EntityList& entities, F32 delta) -> void {
    for (auto& entity : entities) {
      if (entity.movement.speed > 0.0f) {
        fl::Vec2 d{fl::cosine(entity.movement.direction), fl::sine(entity.movement.direction)};
        F32 distance = entity.movement.speed * 0.01f * delta;
        entity.position += d * distance;

        entity.movement.distance_travelled += distance;

        if (entity.movement.distance_travelled > 5.0f) {
          entity.movement.direction = fl::degrees((F32)(std::rand() % 360));
          entity.movement.distance_travelled = 0.0f;
        }
      }
    }
  }
};

#endif  // AD_WORLD_SYSTEMS_MOVEMENT_SYSTEM_H_
