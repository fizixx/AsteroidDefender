#ifndef AD_WORLD_ENTITY_H_
#define AD_WORLD_ENTITY_H_

#include "canvas/Math/Common.h"
#include "canvas/Math/Vec2.h"
#include "legion/Resources/Model.h"

const MemSize kInvalidEntityId = std::numeric_limits<MemSize>::max();

struct EntityId {
  MemSize id = kInvalidEntityId;

  bool isValid() {
    return id != kInvalidEntityId;
  }
};

enum class BuildingType : U32 {
  Unknown = 0x00,
  CommandCenter = 0x01,
  Miner = 0x02,
};

struct Entity {
  ca::Vec2 position = ca::Vec2::zero;

  struct {
    ca::Angle direction = ca::degrees(0.0f);
    F32 speed = 0.0f;

    F32 distanceTravelled = 0.0f;
  } movement;

  struct {
    BuildingType type = BuildingType::Unknown;
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
