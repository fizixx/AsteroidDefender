#include "ad/world/world.h"

#include "canvas/utils/immediate_shapes.h"
#include "construction_controller.h"
#include "nucleus/profiling.h"

World::World() = default;

bool World::initialize(le::ResourceManager* resource_manager) {
  link_model_ = resource_manager->get_render_model("link.obj");
  if (!link_model_) {
    return false;
  }

  miner_laser_model_ = resource_manager->get_render_model("miner_laser.obj");
  if (!miner_laser_model_) {
    return false;
  }

  return true;
}

void World::clear() {
  entities_.clear();
}

EntityId World::add_entity_from_prefab(Entity* prefab, const fl::Vec2& position) {
  auto result = entities_.emplaceBack(*prefab);

  auto& entity = result.element();
  auto entity_id = EntityId{result.index()};

  entity.id = entity_id;

  if (entity.type == EntityType::CommandCenter) {
    command_center_id_ = entity_id;
  }

  entity.position = position;

  // If this entity requires a link, then find a suitable link.
  if (entity.has_flags(ENTITY_FLAG_NEEDS_LINK)) {
    entity.building.linked_to_id = find_closest_to(entity_id, ENTITY_FLAG_LINKABLE);
  }

  if (entity.type == EntityType::Miner) {
    // Select an asteroid target for the miner.
    entity.target = find_closest_to(entity_id, ENTITY_FLAG_MINABLE);
  }

  return entity_id;
}

void World::set_cursor_position(const fl::Vec2& position) {
  cursor_position_ = position;

  update_selected_entity();
}

EntityId World::get_entity_under_cursor() const {
  // This is not a system right now, because the proper solution is probably not to go through each
  // entity to check.

  MemSize id = 0;
  for (const auto& entity : entities_) {
    if (entity.building.selection_radius == 0.0f) {
      continue;
    }

    F32 distance_to_cursor = fl::length(entity.position - cursor_position_);
    if (distance_to_cursor < entity.building.selection_radius) {
      return EntityId{id};
    }
    ++id;
  }

  return EntityId{};
}

void World::tick(F32 delta) {
  resource_system_.tick(entities_, delta);
  movement_system_.tick(entities_, delta);
}

void World::render(ca::Renderer* renderer, le::Camera* camera,
                   ConstructionController* construction_controller) {
  fl::Mat4 projection = fl::Mat4::identity;
  camera->updateProjectionMatrix(&projection);

  fl::Mat4 view = fl::Mat4::identity;
  camera->updateViewMatrix(&view);

  auto projection_and_view = projection * view;

  // Render the entities.

  renderer->state().depth_test(true);

  ca::ImmediateRenderer immediate{renderer};

  for (unsigned i = 0; i < entities_.size(); ++i) {
    PROFILE("item")

    const auto& entity = entities_[i];

    auto translation = fl::translation_matrix(fl::Vec3{entity.position, 0.0f});
    auto rotation = fl::rotation_matrix(fl::Vec3{0.0f, 0.0f, 1.0f}, entity.movement.direction);

    auto mvp =
        projection_and_view * fl::create_model_matrix(translation, rotation, fl::Mat4::identity);

    // Draw the entity circle.

    // TODO: set projection_and_view
    if (entity.building.selection_radius > 0.0f) {
      auto color = ca::Color::red;
      if (EntityId{i} == selected_entity_id_) {
        color = ca::Color::green;
      }
      ca::draw_circle(&immediate, mvp, fl::Vec3::zero, entity.building.selection_radius,
                      static_cast<I32>(entity.building.selection_radius / 0.1f), color);
    }

    // Draw entity model.
    le::renderModel(renderer, *entity.render.model, mvp);

    // Draw the entity's link.
    {
      if (entity.building.linked_to_id.is_valid()) {
        auto& linked_to = entities_[entity.building.linked_to_id.id];

        render_stretched_obj(renderer, projection_and_view, entity.position, linked_to.position,
                             link_model_);
      }
    }

    // Draw a miner's laser.
    if (entity.type == EntityType::Miner && entity.target.is_valid() &&
        entity.mining.time_since_last_cycle < entity.mining.cycle_duration * 0.75f) {
      const auto& target = entities_[entity.target.id];
      render_stretched_obj(renderer, projection_and_view, entity.position, target.position,
                           miner_laser_model_);
    }
  }

  // Render the construction prefab:
  if (construction_controller->is_building()) {
    Entity* prefab = construction_controller->prefab();
    DCHECK(prefab);

    auto model = fl::translation_matrix(fl::Vec3{cursor_position_, 0.0f});
    auto mvp = projection_and_view * model;

    le::renderModel(renderer, *prefab->render.model, mvp);

    {
      // Render a link to the closest entity.
      auto closest_id = find_closest_to(cursor_position_, ENTITY_FLAG_LINKABLE);
      if (closest_id.is_valid()) {
        const auto& closest = entities_[closest_id.id];
        render_stretched_obj(renderer, projection_and_view, cursor_position_, closest.position,
                             link_model_);
      }
    }

    // If we are rendering a miner, then find a target and render a mining laser.
    if (prefab->type == EntityType::Miner) {
      // Render a link to the closest entity.
      auto closest_id = find_closest_to(cursor_position_, ENTITY_FLAG_MINABLE);
      if (closest_id.is_valid()) {
        const auto& closest = entities_[closest_id.id];
        render_stretched_obj(renderer, projection_and_view, cursor_position_, closest.position,
                             miner_laser_model_);
      }
    }
  }

  immediate.submit_to_renderer();
}

void World::update_selected_entity() {
  for (unsigned i = 0; i < entities_.size(); ++i) {
    const auto& entity = entities_[i];

    if (entity.building.selection_radius <= 0.0f) {
      continue;
    }

    auto distance_to_cursor = fl::distance(entity.position, cursor_position_);
    if (distance_to_cursor < entity.building.selection_radius) {
      selected_entity_id_ = EntityId{i};
      return;
    }
  }

  selected_entity_id_ = EntityId{};
}

void World::render_stretched_obj(ca::Renderer* renderer, const fl::Mat4& projection_and_view,
                                 const fl::Vec2& from, const fl::Vec2& to,
                                 le::RenderModel* render_model) const {
  auto distance_to_linked = fl::distance(from, to);
  auto angle = fl::arcTangent2(to.x - from.x, to.y - from.y);

  fl::Mat4 model = fl::Mat4::identity;
  model = model * fl::translation_matrix({from, 0.0f});
  model = model * fl::rotation_matrix(fl::Vec3::forward, fl::Angle::fromRadians(angle));
  model = model * fl::scale_matrix({1.0f, distance_to_linked, 1.0f});

  le::renderModel(renderer, *render_model, projection_and_view * model);
}

EntityId World::find_closest_to(EntityId miner_id, U32 mask) {
  DCHECK(miner_id.is_valid());
  auto& miner = entities_[miner_id.id];

  auto not_including_self = excluding_id(all(), miner_id);
  for (auto& element : not_including_self) {
    LOG(Info) << "not self: " << element.id.id;
  }
  auto is_minable = matching_mask(not_including_self, mask);

  return closest(is_minable, miner.position);
}

EntityId World::find_closest_to(const fl::Vec2& position, U32 mask) {
  auto is_minable = matching_mask(all(), mask);

  return closest(is_minable, position);
}
