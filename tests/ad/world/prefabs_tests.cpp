#include <catch2/catch.hpp>

#include "ad/world/prefabs.h"

TEST_CASE("Prefabs") {
  SECTION("basic") {
    Prefabs prefabs{nullptr};

    prefabs.set(EntityType::CommandCenter, [](le::ResourceManager*, Entity* storage) -> bool {
      storage->building.selection_radius = 10.0f;
      storage->render.model = new le::RenderModel;
      storage->render.model->root_node().children().emplaceBack(le::ModelNode());
      return true;
    });

    Entity* new_prefab = prefabs.get(EntityType::CommandCenter);
    REQUIRE(new_prefab != nullptr);
    CHECK(new_prefab->building.selection_radius == 10.0f);
    CHECK(new_prefab->render.model->root_node().children().size() == 1);
  }
}
