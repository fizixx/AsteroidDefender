#pragma once

#include "ad/World/Prefabs.h"
#include "ad/World/World.h"
#include "floats/Random.h"

class Generator {
public:
  static auto generate(World* world, Prefabs* prefabs) -> void {
    Entity* command_center = prefabs->get(EntityType::CommandCenter);
    Entity* miner = prefabs->get(EntityType::Miner);
    Entity* asteroid = prefabs->get(EntityType::Asteroid);
    Entity* enemy_fighter = prefabs->get(EntityType::EnemyFighter);

    DCHECK(command_center);
    DCHECK(miner);
    DCHECK(asteroid);
    DCHECK(enemy_fighter);

    world->clear();

    auto create_command_center = [&](const fl::Vec2& position) {
      world->add_entity_from_prefab(command_center, position);
    };

    auto create_miner = [&](const fl::Vec2& position) {
      world->add_entity_from_prefab(miner, position);
    };

    auto create_asteroid = [&](const fl::Vec2& position) {
      world->add_entity_from_prefab(asteroid, position);
    };

    auto create_enemy_fighter = [&](const fl::Vec2& position) {
      world->add_entity_from_prefab(enemy_fighter, position);
    };

    create_command_center(fl::Vec2::zero);

#if 0
    create_miner(fl::Vec2{10.0f, 10.0f});
    create_miner(fl::Vec2{5.0f, 10.0f});
    create_miner(fl::Vec2{-5.0f, 5.0f});

    for (U32 i = 0; i < 50; ++i) {
      fl::Angle theta = fl::degrees((F32)(fl::randomInt() % 360));
      F32 distance = (F32)(fl::randomInt() % 100);
      create_asteroid(fl::Vec2{fl::cosine(theta) * distance, fl::sine(theta) * distance});
    }

    for (U32 i = 0; i < 50; ++i) {
      fl::Angle theta = fl::degrees((F32)(fl::randomInt() % 360));
      F32 distance = (F32)(fl::randomInt() % 100);
      create_enemy_fighter(fl::Vec2{fl::cosine(theta) * distance, fl::sine(theta) * distance});
    }
#endif  // 0
  }
};
