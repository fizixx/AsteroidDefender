#pragma once

#include <nucleus/memory/scoped_ptr.h>

#include "floats/common.h"
#include "floats/vec2.h"
#include "legion/resources/render_model.h"
#include "nucleus/hash.h"

DECLARE_RESOURCE_ID(Entity);

enum class EntityType : U32 {
  Unknown = 0,

  // Buildings
  CommandCenter,
  Miner,
  Turret,
  Hub,

  // Static
  Asteroid,

  // Enemies
  EnemyFighter,

  // Count
  Count,
};

constexpr U32 ENTITY_FLAG_NEEDS_LINK = NU_BIT(1);
constexpr U32 ENTITY_FLAG_LINKABLE = NU_BIT(2);
constexpr U32 ENTITY_FLAG_MINABLE = NU_BIT(3);
constexpr U32 ENTITY_FLAG_ENEMY = NU_BIT(4);
constexpr U32 ENTITY_FLAG_ALL = std::numeric_limits<U32>::max();

template <>
struct nu::Hash<EntityType> {
  static HashedValue hashed(EntityType entity_type) {
    return hash_dword(static_cast<U32>(entity_type));
  }
};

struct Entity {
  EntityId id;
  EntityType type = EntityType::Unknown;
  fl::Vec2 position = fl::Vec2::zero;
  U32 flags = 0;

  EntityId target;

  struct Movement {
    fl::Angle direction = fl::Angle::zero;
    F32 speed = 0.0f;

    F32 distance_travelled = 0.0f;
  } movement;

  struct Building {
    EntityId linked_to_id;
    F32 selection_radius = 0.0f;
  } building;

  struct Electricity {
    I32 electricity_delta = 0;
  } electricity;

  struct Mining {
    F32 time_since_last_cycle = 0.0f;
    F32 cycle_duration = 0.0f;
    I32 mineral_amount_per_cycle = 0;
  } mining;

  struct Render {
    le::RenderModel* model = nullptr;
  } render;

  NU_NO_DISCARD bool has_flags(U32 mask) const {
    return NU_BIT_IS_SET(flags, mask);
  }
};

using EntityList = nu::DynamicArray<Entity>;
