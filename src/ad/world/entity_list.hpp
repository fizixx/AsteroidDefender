#pragma once

#include <nucleus/containers/dynamic_array.h>

#include <ranges>

#include "ad/world/entity.h"

namespace ad {

using EntityList = nu::DynamicArray<Entity>;

auto matching_mask(EntityFlags mask) {
  return std::ranges::views::filter(
      [mask](const Entity& entity) { return entity.has_flags(mask); });
}

auto excluding_id(EntityId id) {
  return std::views::filter([id](const Entity& entity) { return entity.id != id; });
}

auto within_radius(const fl::Vec2 center, F32 radius) {
  return std::views::filter([center, radius](Entity& entity) {
    F32 distance_to_entity = fl::distance(center, entity.position);
    return distance_to_entity <= radius;
  });
}

template <std::ranges::input_range Range>
EntityId closest(Range&& range, const fl::Vec2& position) {
  F32 closest_distance = std::numeric_limits<F32>::max();
  EntityId closest_id;

  for (auto& entity : std::forward<Range>(range)) {
    auto distance_to_entity = fl::distance(position, entity.position);
    if (distance_to_entity <= closest_distance) {
      closest_distance = distance_to_entity;
      closest_id = entity.id;
    }
  }

  return closest_id;
}

}  // namespace ad
