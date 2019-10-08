#ifndef AD_WORLD_RESOURCES_H_
#define AD_WORLD_RESOURCES_H_

#include "nucleus/Types.h"

class Resources {
public:
  Resources() = default;

  auto electricity() const -> I32 {
    return m_electricity;
  }

  auto minerals() const -> I32 {
    return m_minerals;
  }

  auto setElectricity(I32 electricity) -> void {
    m_electricity = electricity;
  }

  auto addMinerals(I32 minerals) -> void {
    m_minerals += minerals;
  }

  void reset(I32 electricity, I32 minerals) {
    m_electricity = electricity;
    m_minerals = minerals;
  }

private:
  I32 m_electricity = 0;
  I32 m_minerals = 0;
};

#endif  // AD_WORLD_RESOURCES_H_
