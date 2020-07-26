#include "ad/App/UserInterface.h"

#include <cstdio>

#include "ad/World/ConstructionController.h"
#include "ad/World/World.h"
#include "elastic/Views/ButtonView.h"
#include "elastic/Views/LabelView.h"
#include "elastic/Views/LinearSizerView.h"
#include "nucleus/FilePath.h"
#include "nucleus/Streams/FileInputStream.h"

namespace {

struct BuildClickListener : public el::ButtonView::OnClickListener {
  ConstructionController* constructionController;
  EntityType entityType;

  BuildClickListener(ConstructionController* constructionController, EntityType entityType)
    : constructionController{constructionController}, entityType{entityType} {}

  ~BuildClickListener() override = default;

  void onButtonClicked(el::ButtonView* NU_UNUSED(sender)) override {
    constructionController->startBuilding(entityType);
  }
};

}  // namespace

UserInterface::UserInterface(ConstructionController* constructionController, Resources* resources)
  : m_resources{resources}, m_constructionController{constructionController} {}

auto UserInterface::initialize(ca::Renderer* renderer) -> bool {
  if (!m_ui.initialize(renderer)) {
    return false;
  }

#if OS(POSIX)
  nu::FileInputStream fontStream{nu::FilePath{"/Library/Fonts/Arial Unicode.ttf"}};
#elif OS(MACOSX)
  nu::FileInputStream fontStream{nu::FilePath{R"(/Library/Fonts/Arial.ttf)"}};
#else
  nu::FileInputStream fontStream{nu::FilePath{R"(C:\Windows\Fonts\Arial.ttf)"}};
#endif
  m_font.load(&fontStream, renderer, 30);

  return createUI(&m_ui, &m_font);
}

auto UserInterface::tick(F32 delta) -> void {
  m_ui.tick(delta);

  Char buf[128];

#if COMPILER(MSVC)
  sprintf_s(buf, sizeof(buf), "electricity: %d", m_resources->electricity());
#else
  sprintf(buf, "electricity: %d", m_resources->electricity());
#endif
  m_electricityLabel->setLabel(buf);

#if COMPILER(MSVC)
  sprintf_s(buf, sizeof(buf), "minerals: %d", m_resources->minerals());
#else
  sprintf(buf, "minerals: %d", m_resources->minerals());
#endif
  m_mineralsLabel->setLabel(buf);
}

auto UserInterface::createUI(el::Context* context, el::Font* NU_UNUSED(font)) -> bool {
  el::ContextView* rootView = context->getRootView();

  auto resourcesContainer = new el::LinearSizerView{&m_ui};
  rootView->addChild(resourcesContainer);
  resourcesContainer->setOrientation(el::Orientation::Vertical);
  resourcesContainer->setHorizontalAlignment(el::Alignment::Left);
  resourcesContainer->setVerticalAlignment(el::Alignment::Top);

  m_electricityLabel = new el::LabelView{&m_ui, "0", &m_font};
  //  resourcesContainer->addChild(m_electricityLabel);

  m_mineralsLabel = new el::LabelView{&m_ui, "0", &m_font};
  //  resourcesContainer->addChild(m_mineralsLabel);

  auto buttonContainer = new el::LinearSizerView{&m_ui};
  rootView->addChild(buttonContainer);
  buttonContainer->setOrientation(el::Orientation::Vertical);
  buttonContainer->setHorizontalAlignment(el::Alignment::Left);
  buttonContainer->setVerticalAlignment(el::Alignment::Bottom);
  buttonContainer->setMinSize(ca::Size{250, 0});

  addBuildButton(buttonContainer, EntityType::CommandCenter, "Command Center");
  addBuildButton(buttonContainer, EntityType::Miner, "Miner");

  return true;
}

auto UserInterface::addBuildButton(el::GroupView* container, EntityType entityType,
                                   const nu::StringView& label) -> void {
  auto clickListener = new BuildClickListener{m_constructionController, entityType};
  auto button = new el::ButtonView{&m_ui, label, clickListener};
  button->setFont(&m_font);
  button->setMinSize(ca::Size{0, 45});
  button->setExpansion(el::Expansion::Horizontal);
  container->addChild(button);
}
