#include <elastic/Views/button_view.h>
#include <elastic/context.h>
#include <elastic/views/label_view.h>
#include <elastic/views/linear_sizer_view.h>

#include <legion/engine/engine.hpp>
#include <legion/engine/user_interface_engine_layer.hpp>

#include "World/world.h"

namespace ad {

class UILayer : public le::UserInterfaceEngineLayer {
public:
  explicit UILayer(nu::ScopedRefPtr<Context> context) : context_{std::move(context)} {}

protected:
  bool on_initialize() override {
    if (!context().initialize(&renderer())) {
      return false;
    }

    nu::StringView font_path = "fonts/lato-regular.ttf";
    main_font_ = resource_manager().get_font(font_path, 24);
    if (!main_font_) {
      LOG(Error) << "Could not load font (" << font_path << ")";
      return false;
    }

    auto button_container = new el::LinearSizerView{&context()};
    button_container->setHorizontalAlignment(el::Alignment::Left);
    button_container->setVerticalAlignment(el::Alignment::Bottom);
    button_container->setExpansion(el::Expansion::Horizontal);
    context().root_view()->addChild(button_container);

    build_miner_button_ =
        new el::ButtonView{&context(), "Miner", [this](el::ButtonView* source) {
                             context_->construction_controller().start_building(EntityType::Miner);
                           }};
    build_miner_button_->setFont(main_font_);
    button_container->addChild(build_miner_button_);

    build_turret_button_ = new el::ButtonView{
        &context(), "Turret", [this](el::ButtonView* source) {
          context_->construction_controller().start_building(EntityType::Asteroid);
        }};
    build_turret_button_->setFont(main_font_);
    button_container->addChild(build_turret_button_);

    label_ = new el::LabelView{&context(), "test", main_font_};
    label_->setHorizontalAlignment(el::Alignment::Right);
    label_->setVerticalAlignment(el::Alignment::Bottom);
    context().root_view()->addChild(label_);

    return true;
  }

  void on_resize(const fl::Size& size) override {
    context().resize(size);
  }

  void on_render() override {
    char buf[64];
    sprintf(buf, "%llu", context_->world().selected_entity_id().id);
    label_->setLabel(buf);
    context().render(&renderer());
  }

private:
  nu::ScopedRefPtr<Context> context_;

  el::Font* main_font_ = nullptr;
  el::ButtonView* build_miner_button_ = nullptr;
  el::ButtonView* build_turret_button_ = nullptr;
  el::LabelView* label_ = nullptr;
};

}  // namespace ad
