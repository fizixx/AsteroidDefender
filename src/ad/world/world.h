#pragma once

#include <ad/world/Systems/movement_system.h>
#include <ad/world/Systems/resource_system.h>
#include <ad/world/entity.h>
#include <ad/world/resources.h>
#include <legion/world/camera.h>
#include <nucleus/containers/dynamic_array.h>
#include <nucleus/macros.h>

namespace ca {
class Renderer;
}

namespace hi {
class ResourceManager;
}

class ConstructionController;

namespace detail {

template <typename T>
struct FilteredStorageType {
  using Type = T;
};

template <>
struct FilteredStorageType<EntityList> {
  using Type = EntityList&;
};

}  // namespace detail

template <typename S, typename Predicate>
class Filtered {
public:
  using Source = typename detail::FilteredStorageType<S>::Type;

  class Iterator {
  public:
    using SourceIterator = typename std::remove_reference_t<Source>::Iterator;

    Iterator(SourceIterator begin, SourceIterator end, Predicate predicate)
      : begin_{begin}, end_{end}, predicate_{predicate} {}

    Iterator& operator=(const Iterator& other) {
      begin_ = other.begin_;
      end_ = other.end_;
      predicate_ = other.predicate_;
      return *this;
    }

    bool operator==(const Iterator& other) const {
      return begin_ == other.begin_;
    }

    bool operator!=(const Iterator& other) const {
      return begin_ != other.begin_;
    }

    Iterator& operator++() {
      begin_ = std::find_if(++begin_, end_, predicate_);
      return *this;
    }

    Iterator operator++(int) {
      return Iterator{std::find_if(begin_++, end_, predicate_)};
    }

    Entity& operator*() {
      return *begin_;
    }

  private:
    SourceIterator begin_;
    SourceIterator end_;
    Predicate predicate_;
  };

  Filtered(Source source, Predicate predicate) : source_{source}, predicate_{predicate} {}

  Iterator begin() {
    auto end = source_.end();
    return Iterator{std::find_if(source_.begin(), end, predicate_), end, predicate_};
    // return Iterator{source_.begin(), source_.end(), predicate_};
  }

  Iterator end() {
    auto end = source_.end();
    return Iterator{end, end, predicate_};
  }

private:
  Source source_;
  Predicate predicate_;
};

template <typename Source, typename Predicate>
auto filter(Source source, Predicate predicate) {
  return Filtered<Source, Predicate>{source, predicate};
}

template <typename Source>
auto matching_mask(Source source, U32 mask) {
  struct Op {
    U32 mask;
    bool operator()(Entity& entity) {
      return entity.has_flags(mask);
    }
  };
  return filter(source, Op{mask});
}

template <typename Source>
auto excluding_id(Source source, EntityId id) {
  struct Op {
    EntityId id;

    bool operator()(Entity& entity) {
      return entity.id != id;
    }

    explicit Op(EntityId id) : id{id} {}
  };

  return filter(source, Op{id});
}

template <typename Source>
auto within_radius(Source source, const fl::Vec2 center, F32 radius) {
  return filter(source, [center, radius](Entity& entity) {
    F32 distance_to_entity = fl::distance(center, entity.position);
    return distance_to_entity <= radius;
  });
}

template <typename Source>
EntityId closest(Source source, const fl::Vec2& position) {
  F32 closest_distance = std::numeric_limits<F32>::max();
  EntityId closest_id;

  for (auto& entity : source) {
    auto distance_to_entity = fl::distance(position, entity.position);
    if (distance_to_entity <= closest_distance) {
      closest_distance = distance_to_entity;
      closest_id = entity.id;
    }
  }

  return closest_id;
}

class World {
  NU_DELETE_COPY_AND_MOVE(World);

public:
  World();

  Resources* resources() {
    return &resources_;
  }

  EntityList& entities() {
    return entities_;
  }

  bool initialize(le::ResourceManager* resource_manager);

  void clear();
  EntityId add_entity_from_prefab(Entity* prefab, const fl::Vec2& position);

  void set_cursor_position(const fl::Vec2& position);
  NU_NO_DISCARD EntityId get_entity_under_cursor() const;

  auto all() {
    return Filtered<EntityList&, bool (*)(Entity&)>{entities_, [](Entity&) { return true; }};
  }

  NU_NO_DISCARD EntityId selected_entity_id() const {
    return selected_entity_id_;
  }

  void tick(F32 delta);
  void render(ca::Renderer* renderer, le::Camera* camera,
              ConstructionController* construction_controller);

private:
  void update_selected_entity();

  void render_stretched_obj(ca::Renderer* renderer, const fl::Mat4& projection_and_view,
                            const fl::Vec2& from, const fl::Vec2& to,
                            le::RenderModel* render_model) const;

  EntityId find_closest_to(EntityId entity_id, U32 mask);
  EntityId find_closest_to(const fl::Vec2& position, U32 mask);

  EntityId find_miner_target(EntityId miner_id);
  EntityId find_miner_target(const fl::Vec2& position);

  fl::Vec2 cursor_position_ = fl::Vec2::zero;

  EntityList entities_;
  EntityId selected_entity_id_;

  Resources resources_;

  MovementSystem movement_system_;
  ResourceSystem resource_system_{&resources_};

  le::RenderModel* link_model_ = nullptr;
  le::RenderModel* miner_laser_model_ = nullptr;
  EntityId command_center_id_;
};
