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

    auto createCommandCenter = [&](const fl::Vec2& position) {
      world->addEntityFromPrefab(commandCenter, position);
    };

    auto createMiner = [&](const fl::Vec2& position) {
      world->addEntityFromPrefab(miner, position);
    };

    auto createAsteroid = [&](const fl::Vec2& position) {
      world->addEntityFromPrefab(asteroid, position);
    };

    auto createEnemyFighter = [&](const fl::Vec2& position) {
      world->addEntityFromPrefab(enemyFighter, position);
    };

    createCommandCenter(fl::Vec2::zero);

    createMiner(fl::Vec2{10.0f, 10.0f});
    createMiner(fl::Vec2{5.0f, 10.0f});
    createMiner(fl::Vec2{-5.0f, 5.0f});

    for (U32 i = 0; i < 50; ++i) {
      fl::Angle theta = fl::degrees((F32)(std::rand() % 360));
      F32 distance = (F32)(std::rand() % 100);
      createAsteroid(fl::Vec2{fl::cosine(theta) * distance, fl::sine(theta) * distance});
    }

    for (U32 i = 0; i < 50; ++i) {
      fl::Angle theta = fl::degrees((F32)(std::rand() % 360));
      F32 distance = (F32)(std::rand() % 100);
      createEnemyFighter(fl::Vec2{fl::cosine(theta) * distance, fl::sine(theta) * distance});
    }
  }
};

#endif  // AD_WORLD_GENERATOR_H_
