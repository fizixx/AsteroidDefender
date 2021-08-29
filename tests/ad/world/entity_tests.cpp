#include <catch2/catch.hpp>

#include "ad/world/entity.h"

namespace ad {

TEST_CASE("Entity") {
  SECTION("flags") {
    Entity command_center;
    command_center.flags = ENTITY_FLAG_LINKABLE;

    Entity miner;
    miner.flags = ENTITY_FLAG_NEEDS_LINK;

    CHECK(command_center.has_flags(ENTITY_FLAG_LINKABLE));
    CHECK(!command_center.has_flags(ENTITY_FLAG_NEEDS_LINK));

    CHECK(!miner.has_flags(ENTITY_FLAG_LINKABLE));
    CHECK(miner.has_flags(ENTITY_FLAG_NEEDS_LINK));
  }
}

}  // namespace ad
