#pragma once

#include "nucleus/Types.h"

class Resources {
public:
  Resources() = default;

  NU_NO_DISCARD auto electricity() const -> I32 {
    return electricity_;
  }

  NU_NO_DISCARD auto minerals() const -> I32 {
    return minerals_;
  }

  auto set_electricity(I32 electricity) -> void {
    electricity_ = electricity;
  }

  auto add_minerals(I32 minerals) -> void {
    minerals_ += minerals;
  }

  void reset(I32 electricity, I32 minerals) {
    electricity_ = electricity;
    minerals_ = minerals;
  }

private:
  I32 electricity_ = 0;
  I32 minerals_ = 0;
};
