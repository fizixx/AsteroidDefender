#ifndef AD_APP_USER_INTERFACE_H_
#define AD_APP_USER_INTERFACE_H_

#include "ad/World/ConstructionController.h"
#include "ad/World/Entity.h"
#include "elastic/Context.h"
#include "elastic/Renderer/Font.h"
#include "nucleus/Macros.h"

namespace el {
class LabelView;
}

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

#endif  // AD_APP_USER_INTERFACE_H_
