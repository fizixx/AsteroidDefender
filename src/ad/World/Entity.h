#pragma once

#include "floats/Common.h"
#include "floats/Vec2.h"
#include "legion/Resources/Model.h"

const MemSize k_invalid_entity_id = std::numeric_limits<MemSize>::max();

struct EntityId {
  MemSize id = k_invalid_entity_id;

  NU_NO_DISCARD bool is_valid() const {
    return id != k_invalid_entity_id;
  }
};

enum class EntityType : U32 {
  Unknown = 0,

  // Buildings
  CommandCenter,
  Miner,

  // Static
  Asteroid,

  // Enemies
  EnemyFighter,

  // Count
  Count,
};

struct Entity {
  EntityType type = EntityType::Unknown;
  fl::Vec2 position = fl::Vec2::zero;

  struct Movement {
    fl::Angle direction = fl::Angle::zero;
    F32 speed = 0.0f;

    F32 distance_travelled = 0.0f;
  } movement;

  struct Building {
    F32 selection_radius = 0.0f;
  } building;

  struct Electricity{
    I32 electricity_delta = 0;
  } electricity;

  struct Mining {
    F32 time_since_last_cycle = 0.0f;
    F32 cycle_duration = 0.0f;
    I32 mineral_amount_per_cycle = 0;
  } mining;

  struct Render {
    le::Model* model = nullptr;
  } render;
};

using EntityList = nu::DynamicArray<Entity>;
