#include <catch2/catch.hpp>

#include "ad/world/world.h"

namespace ad {

TEST_CASE("World") {
  SECTION("EntityView") {
    Entity prefab;

    World world;

    prefab.type = EntityType::CommandCenter;
    prefab.flags = 0;
    world.add_entity_from_prefab(&prefab, fl::Vec2::zero);

    prefab.type = EntityType::Miner;
    prefab.flags = 0;
    world.add_entity_from_prefab(&prefab, fl::Vec2::zero);

    prefab.type = EntityType::Asteroid;
    prefab.flags = ENTITY_FLAG_MINABLE;
    world.add_entity_from_prefab(&prefab, fl::Vec2::zero);

    prefab.type = EntityType::Miner;
    prefab.flags = 0;
    world.add_entity_from_prefab(&prefab, fl::Vec2::zero);

#if 0
    auto view = world.filter([](auto& entity) { return true; });
    for (auto& entity : view) {
      LOG(Info) << "entity: " << entity.id.id;
    }
#endif  // 0

#if 0
    auto view = world.all();
    auto view_2 = filter(view, [](Entity& e) { return e.type == EntityType::Miner; });
    for (auto& entity : view_2) {
      LOG(Info) << entity.id.id;
    }
#endif  // 0

    auto minable = matching_mask(world.all(), ENTITY_FLAG_MINABLE);
    for (auto& e : minable) {
      LOG(Info) << "minable: " << e.id.id;
    }
  }
}

}  // namespace ad
