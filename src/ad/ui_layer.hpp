#include <elastic/Views/button_view.h>
#include <elastic/context.h>

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

    main_button_ = new el::ButtonView{&context(), "Main Button",
                                      [](auto* source) { LOG(Info) << "Button clicked"; }};
    main_button_->setFont(main_font_);
    context().root_view()->addChild(main_button_);
    return true;
  }

  void on_resize(const fl::Size& size) override {
    context().resize(size);
  }

  void on_render() override {
    context().render(&renderer());
  }

private:
  nu::ScopedRefPtr<Context> context_;

  el::Font* main_font_ = nullptr;
  el::ButtonView* main_button_ = nullptr;
};

}  // namespace ad
