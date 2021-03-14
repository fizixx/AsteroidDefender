#ifndef AD_WORLD_ENTITY_H_
#define AD_WORLD_ENTITY_H_

#include "floats/Common.h"
#include "floats/Vec2.h"
#include "legion/Resources/Model.h"

const MemSize kInvalidEntityId = std::numeric_limits<MemSize>::max();

struct EntityId {
  MemSize id = kInvalidEntityId;

  bool isValid() {
    return id != kInvalidEntityId;
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

  struct {
    fl::Angle direction = fl::Angle::zero;
    F32 speed = 0.0f;

    F32 distanceTravelled = 0.0f;
  } movement;

  struct {
    F32 selectionRadius = 0.0f;
  } building;

  struct {
    I32 electricityDelta = 0;
  } electricity;

  struct {
    F32 timeSinceLastCycle = 0.0f;
    F32 cycleDuration = 0.0f;
    I32 mineralAmountPerCycle = 0;
  } mining;

  struct {
    le::Model* model = nullptr;
  } render;
};

using EntityList = nu::DynamicArray<Entity>;

#endif  // AD_WORLD_ENTITY_H_
