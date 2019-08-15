#include "ad/Sprites/SpriteConverter.h"
#include "ad/Sprites/SpriteRenderer.h"
#include "ad/World/CameraInputController.h"
#include "ad/World/World.h"
#include "canvas/App.h"
#include "canvas/Math/Intersection.h"
#include "elastic/Context.h"
#include "elastic/Views/LabelView.h"
#include "hive/PhysicalResourceLocator.h"
#include "hive/ResourceManager.h"
#include "nucleus/Streams/FileInputStream.h"

class AsteroidDefender : public ca::WindowDelegate {
public:
  AsteroidDefender() : ca::WindowDelegate{"Asteroid Defender"} {}
  ~AsteroidDefender() override = default;

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

    m_worldCamera.moveTo({0.0f, 0.0f, 150.0f});
    m_worldCamera.rotate(-90.0f, 0.0f);

    return true;
  }

  void onWindowResized(const ca::Size& size) override {
    WindowDelegate::onWindowResized(size);

    m_screenSize = ca::Vec2{static_cast<F32>(size.width), static_cast<F32>(size.height)};

    m_worldCamera.resize(size);
    m_spriteRenderer.resize(size);
    m_ui.resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& event) override {
    if (m_useDebugCamera) {
      m_debugCameraInputController.onMouseMoved(event);
    } else {
      m_worldCameraInputController.onMouseMoved(event);
    }

    m_cursorPosition = {static_cast<F32>(event.pos.x), static_cast<F32>(event.pos.y)};
  }

  bool onMousePressed(const ca::MouseEvent& event) override {
    if (m_useDebugCamera) {
      m_debugCameraInputController.onMousePressed(event);
    } else {
      m_worldCameraInputController.onMousePressed(event);
    }

    return false;
  }

  void onMouseReleased(const ca::MouseEvent& event) override {
    if (m_useDebugCamera) {
      m_debugCameraInputController.onMouseReleased(event);
    } else {
      m_worldCameraInputController.onMouseReleased(event);
    }
  }

  void onMouseWheel(const ca::MouseWheelEvent& evt) override {
    m_worldCamera.moveRelative({0.0f, 0.0f, -evt.wheelOffset.y * 10.0f});
  }

  void onKeyPressed(const ca::KeyEvent& evt) override {
    if (m_useDebugCamera) {
      m_debugCameraInputController.onKeyPressed(evt);
    } else {
      m_worldCameraInputController.onKeyPressed(evt);
    }

    switch (evt.key) {
      case ca::Key::C:
        m_useDebugCamera = !m_useDebugCamera;
        m_cameraLabel->setLabel(m_useDebugCamera ? "debug camera" : "world camera");
        break;

      default:
        break;
    }
  }

  void onKeyReleased(const ca::KeyEvent& evt) override {
    if (m_useDebugCamera) {
      m_debugCameraInputController.onKeyReleased(evt);
    } else {
      m_worldCameraInputController.onKeyReleased(evt);
    }
  }

  void tick(F32 delta) override {
    if (m_useDebugCamera) {
      m_debugCameraInputController.tick(delta);
    } else {
      m_worldCameraInputController.tick(delta);
    }

    m_world.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    m_spriteRenderer.beginFrame(m_useDebugCamera ? &m_debugCamera : &m_worldCamera);
    m_world.render(&m_spriteRenderer);
    m_ui.render(renderer);
  }

private:
  DELETE_COPY_AND_MOVE(AsteroidDefender);

  bool createUI(el::Context* context, el::Font* font) {
    m_cameraLabel = new el::LabelView{context, "world camera", font};
    context->getRootView()->addChild(m_cameraLabel);
    m_cameraLabel->setVerticalAlignment(el::Alignment::Top);
    m_cameraLabel->setHorizontalAlignment(el::Alignment::Left);

    return true;
  }

  hi::ResourceManager m_resourceManager;
  hi::PhysicalFileResourceLocator m_physicalFileResourceLocator;
  SpriteConverter m_spriteConverter;

  el::Font m_font;
  el::Context m_ui;

  el::LabelView* m_cameraLabel = nullptr;

  SpriteRenderer m_spriteRenderer;
  Camera m_worldCamera;
  World m_world;

  bool m_useDebugCamera = false;
  Camera m_debugCamera;

  CameraInputController m_worldCameraInputController{&m_worldCamera, 0.1f};
  CameraInputController m_debugCameraInputController{&m_debugCamera, 0.1f};

  ca::Vec3 m_moveDelta{0.0f, 0.0f, 0.0f};

  // Client size of the area we're rendering the world into:
  ca::Vec2 m_screenSize{0.0f, 0.0f};

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  ca::Vec2 m_cursorPosition{0.0f, 0.0f};
};

CANVAS_APP(AsteroidDefender)
