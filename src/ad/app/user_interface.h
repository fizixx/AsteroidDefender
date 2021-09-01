#pragma once

#include <elastic/context.h>
#include <elastic/renderer/font.h>
#include <nucleus/macros.h>

#include "ad/world/construction_controller.h"
#include "ad/world/entity.h"

namespace el {
class LabelView;
}

namespace ad {

class World;

class UserInterface {
public:
  NU_DELETE_COPY_AND_MOVE(UserInterface);

  UserInterface(ConstructionController* construction_controller, Resources* resources);

  auto initialize(ca::Renderer* renderer) -> bool;
  auto tick(F32 delta) -> void;

  el::Context& ui() {
    return ui_;
  }

private:
  auto create_ui(el::Context* context, el::Font* font) -> bool;
  auto add_build_button(el::GroupView* container, EntityType entity_type,
                        const nu::StringView& label) -> void;

  el::Font font_;
  el::Context ui_;

  el::Image button_background_;

  el::LabelView* electricity_label_ = nullptr;
  el::LabelView* minerals_label_ = nullptr;

  Resources* resources_;
  ConstructionController* construction_controller_;
};

}  // namespace ad
