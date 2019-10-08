#ifndef AD_APP_USER_INTERFACE_H_
#define AD_APP_USER_INTERFACE_H_

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
  explicit UserInterface(World* world);

  auto initialize(ca::Renderer* renderer) -> bool;
  auto tick(F32 delta) -> void;

  el::Context& ui() {
    return m_ui;
  }

private:
  DELETE_COPY_AND_MOVE(UserInterface);

  auto createUI(el::Context* context, el::Font* font) -> bool;
  auto addBuildButton(el::GroupView* container, BuildingType buildingType,
                      const nu::StringView& label) -> void;

  el::Font m_font;
  el::Context m_ui;

  el::LabelView* m_electricityLabel;
  el::LabelView* m_mineralsLabel;

  World* m_world;
};

#endif  // AD_APP_USER_INTERFACE_H_
