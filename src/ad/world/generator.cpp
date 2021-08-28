#include "ad/world/generator.hpp"

#include <floats/random.h>

bool populate_world(World* world, Prefabs* prefabs) {
  Entity* command_center = prefabs->get(EntityType::CommandCenter);
  if (!command_center) {
    LOG(Error) << "Could not load command center prefab.";
    return false;
  }

  Entity* miner = prefabs->get(EntityType::Miner);
  if (!miner) {
    LOG(Error) << "Could not load miner prefab.";
    return false;
  }

  Entity* asteroid = prefabs->get(EntityType::Asteroid);
  if (!asteroid) {
    LOG(Error) << "Could not load asteroid prefab.";
    return false;
  }

  Entity* enemy_fighter = prefabs->get(EntityType::EnemyFighter);
  if (!enemy_fighter) {
    LOG(Error) << "Could not load enemy fighter prefab.";
    return false;
  }

  world->clear();

  auto create_command_center = [&](const fl::Vec2& position) {
    return world->add_entity_from_prefab(command_center, position);
  };

  auto create_miner = [&](const fl::Vec2& position) {
    return world->add_entity_from_prefab(miner, position);
  };

  auto create_asteroid = [&](const fl::Vec2& position) {
    return world->add_entity_from_prefab(asteroid, position);
  };

  auto create_enemy_fighter = [&](const fl::Vec2& position) {
    return world->add_entity_from_prefab(enemy_fighter, position);
  };

  // Command center

  create_command_center(fl::Vec2::zero);

  // Miners

#if 0
  create_miner(fl::Vec2{10.0f, 10.0f});
  create_miner(fl::Vec2{5.0f, 10.0f});
  create_miner(fl::Vec2{-5.0f, 5.0f});
#endif  // 0

  // Asteroids

#if 0
  for (U32 i = 0; i < 10; ++i) {
    fl::Angle theta = fl::degrees((F32)(fl::randomInt() % 360));
    F32 distance = (F32)(fl::randomInt() % 100);
    create_asteroid(fl::Vec2{fl::cosine(theta) * distance, fl::sine(theta) * distance});
  }
#endif  // 0

  // Enemy fighters

#if 0
  for (U32 i = 0; i < 20; ++i) {
    fl::Angle theta = fl::degrees((F32)(fl::randomInt() % 360));
    F32 distance = (F32)(fl::randomInt() % 100);
    create_enemy_fighter(fl::Vec2{fl::cosine(theta) * distance, fl::sine(theta) * distance});
  }
#endif  // 0

  return true;
}
