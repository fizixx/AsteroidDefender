#include "ad/Sprites/SpriteConverter.h"
#include "ad/Sprites/SpriteRenderer.h"
#include "ad/World/World.h"

#include "canvas/App.h"
#include "elastic/Context.h"
#include "elastic/Views/LabelView.h"
#include "hive/PhysicalResourceLocator.h"
#include "hive/ResourceManager.h"
#include "nucleus/Streams/FileInputStream.h"

class AsteroidDefender : public ca::WindowDelegate {
public:
  AsteroidDefender() : ca::WindowDelegate{"Asteroid Defender"} {}
  ~AsteroidDefender() override = default;

  // Override: ca::WindowDelegate

  bool onWindowCreated(ca::Window* window) override {
    if (!WindowDelegate::onWindowCreated(window)) {
      return false;
    }

    ca::Renderer* renderer = window->getRenderer();

    auto assetsPath = nu::getCurrentWorkingDirectory() / "assets";
    LOG(Info) << "Assets path: " << assetsPath.getPath();
    m_physicalFileResourceLocator.setRootPath(assetsPath);
    m_resourceManager.addResourceLocatorBack(&m_physicalFileResourceLocator);

    m_spriteConverter.setRenderer(renderer);
    m_resourceManager.registerConverter(&m_spriteConverter);

    if (!m_ui.initialize(renderer)) {
      return false;
    }

#if OS(MACOSX)
    nu::FileInputStream fontStream{nu::FilePath { R"(/Library/Fonts/Arial.ttf)" }};
#else
    nu::FileInputStream fontStream{nu::FilePath{R"(C:\Windows\Fonts\Arial.ttf)"}};
#endif
    m_font.load(&fontStream, renderer, 20);

    if (!createUI(&m_ui, &m_font)) {
      return false;
    }

    if (!m_spriteRenderer.initialize(renderer)) {
      return false;
    }

    if (!m_world.initialize(&m_resourceManager)) {
      return false;
    }

    return true;
  }

  void onWindowResized(const ca::Size& size) override {
    WindowDelegate::onWindowResized(size);

    m_camera.resize(size);
    m_spriteRenderer.resize(size);
    m_ui.resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& evt) override {}

  bool onMousePressed(const ca::MouseEvent& evt) override {
    return false;
  }

  void onMouseReleased(const ca::MouseEvent& evt) override {}

  void onMouseWheel(const ca::MouseWheelEvent& evt) override {
    m_camera.zoomRelative(-evt.wheelOffset.y * 10.0f);
  }

  void onKeyPressed(const ca::KeyEvent& evt) override {
    switch (evt.key) {
      case ca::Key::A:
        m_moveDelta += {1.0f, 0.0f};
        break;

      case ca::Key::D:
        m_moveDelta += {-1.0f, 0.0f};
        break;

      case ca::Key::W:
        m_moveDelta += {0.0f, -1.0f};
        break;

      case ca::Key::S:
        m_moveDelta += {0.0f, 1.0f};
        break;
    }
  }

  void onKeyReleased(const ca::KeyEvent& evt) override {
    switch (evt.key) {
    case ca::Key::A:
      m_moveDelta -= {1.0f, 0.0f};
      break;

    case ca::Key::D:
      m_moveDelta -= {-1.0f, 0.0f};
      break;

    case ca::Key::W:
      m_moveDelta -= {0.0f, -1.0f};
      break;

    case ca::Key::S:
      m_moveDelta -= {0.0f, 1.0f};
      break;
    }
  }

  void tick(F32 delta) override {
    m_camera.moveRelative(m_moveDelta);
    m_camera.tick(delta);
    m_world.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    m_spriteRenderer.beginFrame(&m_camera);
    m_world.render(&m_spriteRenderer);
    m_ui.render(renderer);
  }

private:
  DELETE_COPY_AND_MOVE(AsteroidDefender);

  static bool createUI(el::Context* context, el::Font* font) {
    auto lv = new el::LabelView{context, "Asteroid Defender", font};
    context->getRootView()->addChild(lv);

    return true;
  }

  hi::ResourceManager m_resourceManager;
  hi::PhysicalFileResourceLocator m_physicalFileResourceLocator;
  SpriteConverter m_spriteConverter;

  el::Font m_font;
  el::Context m_ui;

  SpriteRenderer m_spriteRenderer;
  Camera m_camera;
  World m_world;

  ca::Vec2 m_moveDelta{0.0f, 0.0f};
};

CANVAS_APP(AsteroidDefender)
