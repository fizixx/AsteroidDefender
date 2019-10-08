#ifndef AD_WORLD_SYSTEMS_RESOURCE_SYSTEM_H_
#define AD_WORLD_SYSTEMS_RESOURCE_SYSTEM_H_

#include <ad/World/Entity.h>
#include "ad/World/Entity.h"
#include "ad/World/Resources.h"

struct ResourceSystem {
  Resources* resources;

  explicit ResourceSystem(Resources* resources) : resources{resources} {}

  auto tick(EntityList& entities, F32 delta) -> void {
    I32 totalMinerals = 0;
    I32 totalElectricity = 0;

    for (auto& entity : entities) {
      // Electricity

      totalElectricity += entity.electricity.electricityDelta;

      // Mining

      entity.mining.timeSinceLastCycle += delta;
      if (entity.mining.timeSinceLastCycle >= entity.mining.cycleDuration) {
        entity.mining.timeSinceLastCycle -= entity.mining.cycleDuration;

        totalMinerals += entity.mining.mineralAmountPerCycle;
      }
    }

    resources->setElectricity(totalElectricity);
    resources->addMinerals(totalMinerals);
  }
};

#endif  // AD_WORLD_SYSTEMS_RESOURCE_SYSTEM_H_
