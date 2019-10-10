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
  UserInterface(ConstructionController* constructionController, Resources* resources);

  auto initialize(ca::Renderer* renderer) -> bool;
  auto tick(F32 delta) -> void;

  el::Context& ui() {
    return m_ui;
  }

private:
  DELETE_COPY_AND_MOVE(UserInterface);

  auto createUI(el::Context* context, el::Font* font) -> bool;
  auto addBuildButton(el::GroupView* container, EntityType entityType, const nu::StringView& label)
      -> void;

  el::Font m_font;
  el::Context m_ui;

  el::LabelView* m_electricityLabel;
  el::LabelView* m_mineralsLabel;

  Resources* m_resources;
  ConstructionController* m_constructionController;
};

#endif  // AD_APP_USER_INTERFACE_H_
