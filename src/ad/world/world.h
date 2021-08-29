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

class World {
  NU_DELETE_COPY_AND_MOVE(World);

public:
  World();

  Resources* resources() {
    return &resources_;
  }

  bool initialize(le::ResourceManager* resource_manager);

  void clear();
  EntityId add_entity_from_prefab(Entity* prefab, const fl::Vec2& position);

  void set_cursor_position(const fl::Vec2& position);
  NU_NO_DISCARD EntityId get_entity_under_cursor() const;

  NU_NO_DISCARD EntityId find_closest_to(EntityId entity, U32 mask = 0) const;
  NU_NO_DISCARD EntityId find_closest_to(const fl::Vec2& position, U32 mask = 0) const;

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
