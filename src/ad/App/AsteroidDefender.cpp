#include "ad/Sprites/SpriteConverter.h"
#include "ad/Sprites/SpriteRenderer.h"
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
    m_worldCamera.reorient({0.0f, 0.0f, -1.0f});

    return true;
  }

  void onWindowResized(const ca::Size& size) override {
    WindowDelegate::onWindowResized(size);

    m_screenSize = ca::Vec2{static_cast<F32>(size.width), static_cast<F32>(size.height)};

    m_worldCamera.resize(size);
    m_spriteRenderer.resize(size);
    m_ui.resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& evt) override {
    m_cursorPosition = {static_cast<F32>(evt.pos.x), static_cast<F32>(evt.pos.y)};
  }

  bool onMousePressed(const ca::MouseEvent& UNUSED(evt)) override {
    return false;
  }

  void onMouseReleased(const ca::MouseEvent& UNUSED(evt)) override {}

  void onMouseWheel(const ca::MouseWheelEvent& evt) override {
    m_worldCamera.moveRelative({0.0f, 0.0f, -evt.wheelOffset.y * 10.0f});
  }

  void onKeyPressed(const ca::KeyEvent& evt) override {
    switch (evt.key) {
      case ca::Key::A:
        m_moveDelta += {1.0f, 0.0f, 0.0f};
        break;

      case ca::Key::D:
        m_moveDelta += {-1.0f, 0.0f, 0.0f};
        break;

      case ca::Key::W:
        m_moveDelta += {0.0f, -1.0f, 0.0f};
        break;

      case ca::Key::S:
        m_moveDelta += {0.0f, 1.0f, 0.0f};
        break;

      case ca::Key::C:
        m_useDebugCamera = !m_useDebugCamera;
        break;

      default:
        break;
    }
  }

  void onKeyReleased(const ca::KeyEvent& evt) override {
    switch (evt.key) {
      case ca::Key::A:
        m_moveDelta -= {1.0f, 0.0f, 0.0f};
        break;

      case ca::Key::D:
        m_moveDelta -= {-1.0f, 0.0f, 0.0f};
        break;

      case ca::Key::W:
        m_moveDelta -= {0.0f, -1.0f, 0.0f};
        break;

      case ca::Key::S:
        m_moveDelta -= {0.0f, 1.0f, 0.0f};
        break;

      default:
        break;
    }
  }

  void tick(F32 delta) override {
    m_worldCamera.moveRelative(m_moveDelta);
    m_worldCamera.tick(delta);

#if 0
    ca::Mat4 cameraInverse = ca::inverse(m_view * m_projection);

  F32 mouseX = (2.0f * screenPosition.x) / m_size.x - 1.0f;
  F32 mouseY = (2.0f * screenPosition.y) / m_size.y - 1.0f;
  F32 mouseZ = -1.0f;

  ca::Vec4 mouse = ca::Vec4{mouseX, mouseY, mouseZ, 1.0f};

  ca::Vec4 mouseInWorldSpace = cameraInverse * mouse;
  mouseInWorldSpace.x /= mouseInWorldSpace.w;
  mouseInWorldSpace.y /= mouseInWorldSpace.w;
  mouseInWorldSpace.z /= mouseInWorldSpace.w;
  mouseInWorldSpace.w /= mouseInWorldSpace.w;

  ca::Plane plane{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
  auto intersection = ca::intersection(
      plane,
      {m_current.position, (m_current.position + ca::Vec3{mouseInWorldSpace.x, mouseInWorldSpace.y,
                                                          mouseInWorldSpace.z}) -
                               m_current.position});

  LOG(Info) << "mouse: " << mouse << ", mouseInWorldSpace: " << mouseInWorldSpace;

  return {intersection.position.x, intersection.position.y};
#endif  // 0

#if 1
#if 0
    ca::Vec2 normalizedCursorPosition{
        (2.0f * m_cursorPosition.x) / m_screenSize.x - 1.0f,
        (-2.0f * m_cursorPosition.y) / m_screenSize.y + 1.0f,
    };
#else
    ca::Vec2 normalizedCursorPosition{1.0f, 0.0f};
#endif
    auto ray = m_worldCamera.createRayForMouse(normalizedCursorPosition);
#else
    auto ray = m_WorldCamera.createRay();
#endif  // 0

    ca::Plane worldGround{{0.0f, 0.0f, 1.0f}, 0.0f};
    auto i = ca::intersection(worldGround, ray);
    m_world.setCursorPosition({i.position.x, i.position.y});

    m_world.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    m_spriteRenderer.beginFrame(m_useDebugCamera ? &m_debugCamera : &m_worldCamera);
    m_world.render(&m_spriteRenderer);
    m_ui.render(renderer);
  }

private:
  DELETE_COPY_AND_MOVE(AsteroidDefender);

  static bool createUI(el::Context* context, el::Font* font) {
    auto lv = new el::LabelView{context, "Asteroid Defender", font};
    context->getRootView()->addChild(lv);
    lv->setVerticalAlignment(el::Alignment::Top);

    return true;
  }

  hi::ResourceManager m_resourceManager;
  hi::PhysicalFileResourceLocator m_physicalFileResourceLocator;
  SpriteConverter m_spriteConverter;

  el::Font m_font;
  el::Context m_ui;

  SpriteRenderer m_spriteRenderer;
  Camera m_worldCamera;
  World m_world;

  bool m_useDebugCamera = false;
  Camera m_debugCamera;

  ca::Vec3 m_moveDelta{0.0f, 0.0f, 0.0f};

  // Client size of the area we're rendering the world into:
  ca::Vec2 m_screenSize{0.0f, 0.0f};

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  ca::Vec2 m_cursorPosition{0.0f, 0.0f};
};

CANVAS_APP(AsteroidDefender)
