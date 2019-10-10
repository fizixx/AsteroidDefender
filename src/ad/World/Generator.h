#ifndef AD_WORLD_GENERATOR_H_
#define AD_WORLD_GENERATOR_H_

#include "ad/World/Prefabs.h"
#include "ad/World/World.h"

class Generator {
public:
  auto generate(World* world, Prefabs* prefabs) -> void {
    Entity* commandCenter = prefabs->get(EntityType::CommandCenter);
    Entity* miner = prefabs->get(EntityType::Miner);
    Entity* asteroid = prefabs->get(EntityType::Asteroid);
    Entity* enemyFighter = prefabs->get(EntityType::EnemyFighter);

    world->clear();

    auto createCommandCenter = [&](const ca::Vec2& position) {
      world->addEntityFromPrefab(commandCenter, position);
    };

    auto createMiner = [&](const ca::Vec2& position) {
      world->addEntityFromPrefab(miner, position);
    };

    auto createAsteroid = [&](const ca::Vec2& position) {
      world->addEntityFromPrefab(asteroid, position);
    };

    auto createEnemyFighter = [&](const ca::Vec2& position) {
      world->addEntityFromPrefab(enemyFighter, position);
    };

    createCommandCenter(ca::Vec2::zero);

    createMiner(ca::Vec2{10.0f, 10.0f});
    createMiner(ca::Vec2{5.0f, 10.0f});
    createMiner(ca::Vec2{-5.0f, 5.0f});

    for (U32 i = 0; i < 50; ++i) {
      ca::Angle theta = ca::degrees((F32)(std::rand() % 360));
      F32 distance = (F32)(std::rand() % 100);
      createAsteroid(ca::Vec2{ca::cosine(theta) * distance, ca::sine(theta) * distance});
    }

    for (U32 i = 0; i < 50; ++i) {
      ca::Angle theta = ca::degrees((F32)(std::rand() % 360));
      F32 distance = (F32)(std::rand() % 100);
      createEnemyFighter(ca::Vec2{ca::cosine(theta) * distance, ca::sine(theta) * distance});
    }
  }
};

#endif  // AD_WORLD_GENERATOR_H_
