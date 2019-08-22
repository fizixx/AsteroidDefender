#include "ad/Sprites/SpriteConverter.h"
#include "ad/Sprites/SpriteRenderer.h"
#include "ad/World/CameraInputController.h"
#include "ad/World/World.h"
#include "canvas/App.h"
#include "canvas/Math/Intersection.h"
#include "canvas/Math/Transform.h"
#include "canvas/Renderer/LineRenderer.h"
#include "elastic/Context.h"
#include "elastic/Views/LabelView.h"
#include "hive/PhysicalResourceLocator.h"
#include "hive/ResourceManager.h"
#include "nucleus/Streams/FileInputStream.h"

void drawCross(ca::LineRenderer* lineRenderer, const ca::Vec3& position, const ca::Color& color,
               F32 scale = 1.0f) {
  lineRenderer->renderLine(position - ca::Vec3{1.0f, 0.0f, 0.0f} * scale,
                           position + ca::Vec3{1.0f, 0.0f, 0.0f} * scale, color);
  lineRenderer->renderLine(position - ca::Vec3{0.0f, 1.0f, 0.0f} * scale,
                           position + ca::Vec3{0.0f, 1.0f, 0.0f} * scale, color);
  lineRenderer->renderLine(position - ca::Vec3{0.0f, 0.0f, 1.0f} * scale,
                           position + ca::Vec3{0.0f, 0.0f, 1.0f} * scale, color);
}

class AsteroidDefender : public ca::WindowDelegate {
public:
  AsteroidDefender()
    : ca::WindowDelegate{"Asteroid Defender"}, m_ray{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}} {}
  ~AsteroidDefender() override = default;

  bool onWindowCreated(ca::Window* window) override {
    if (!WindowDelegate::onWindowCreated(window)) {
      return false;
    }

    ca::Renderer* renderer = window->getRenderer();
    m_lineRenderer.initialize(renderer);

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

    m_worldCamera.moveTo({0.0f, 0.0f, 50.0f});
    // m_worldCamera.yaw(ca::Angle::fromDegrees(90.0f));
    // m_worldCamera.yaw(ca::Angle::fromDegrees(0.0f));
    m_debugCamera.moveTo(m_worldCamera.position());
    // m_debugCamera.rotate(180.0f, 0.0f);

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
      m_currentMousePosition = {static_cast<F32>(event.pos.x), static_cast<F32>(event.pos.y)};
    }
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
    m_worldCamera.move({0.0f, 0.0f, -evt.wheelOffset.y * 10.0f});
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

    {
      m_ray = m_worldCamera.createRayForMouse(m_currentMousePosition);

      LOG(Info) << "origin = " << m_ray.origin << ", direction = " << m_ray.direction;

      ca::Plane worldPlane{{0.0f, 0.0f, 1.0f}, 0.0f};
      auto result = ca::intersection(worldPlane, m_ray);
      m_world.setCursorPosition({result.position.x, result.position.y});
    }

    m_world.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    m_lineRenderer.beginFrame();
    m_spriteRenderer.beginFrame(m_useDebugCamera ? &m_debugCamera : &m_worldCamera);

    m_world.render(&m_spriteRenderer);

    //    m_lineRenderer.renderLine({0.0f, 0.0f, 0.0f}, m_worldCamera.position(), ca::Color::blue);
    //    m_lineRenderer.renderLine(m_ray.origin, m_ray.origin + m_ray.direction * 100.0f,
    //                              ca::Color::red);
    //    m_lineRenderer.renderLine(m_worldCamera.position(),
    //                              m_worldCamera.position() + ca::Vec3{1000.0f, 1000.0f, -1000.0f},
    //                              ca::Color::white);
    //    m_lineRenderer.renderLine(m_worldCamera.position(),
    //                              m_worldCamera.position() + ca::Vec3{1000.0f, -1000.0f,
    //                              -1000.0f}, ca::Color::white);
    //    m_lineRenderer.renderLine(m_worldCamera.position(),
    //                              m_worldCamera.position() + ca::Vec3{-1000.0f, -1000.0f,
    //                              -1000.0f}, ca::Color::white);
    //    m_lineRenderer.renderLine(m_worldCamera.position(),
    //                              m_worldCamera.position() + ca::Vec3{-1000.0f, 1000.0f,
    //                              -1000.0f}, ca::Color::white);

    //    m_lineRenderer.renderLine({0.0f, 0.0f, 0.0f}, {1000.0f, 1000.0f, -1000.0f},
    //    ca::Color::red); m_lineRenderer.renderLine({0.0f, 0.0f, 0.0f}, {800.0f, 800.0f, -800.0f},
    //    ca::Color::blue);

    // drawCross(&m_lineRenderer, m_worldCamera.position(), ca::Color::white);
    // drawCross(&m_lineRenderer, m_ray.origin + m_ray.direction, ca::Color::red);
    // drawCross(&m_lineRenderer, {0.0f, 0.0f, 0.0f}, ca::Color::white, 0.5f);

    // ca::Vec3 mPos{0.99999f, 0.99999f, -0.99999f};
    // drawCross(&m_lineRenderer, mPos, ca::Color::blue);

#if 0

    m_lineRenderer.render(ca::Mat4::identity);

    m_lineRenderer.beginFrame();

    auto inv = ca::inverse(m_worldCamera.viewMatrix() * m_worldCamera.projectionMatrix());
    auto newPos = ca::Vec4{mPos, 1.0f} * inv;
    LOG(Info) << "newPos = " << newPos;
    drawCross(&m_lineRenderer, {newPos.x, newPos.y, newPos.z}, ca::Color::green);

#if 1
    ca::Mat4 debugTransform = m_debugCamera.projectionMatrix() * m_debugCamera.viewMatrix();
    ca::Mat4 worldTransform = m_worldCamera.projectionMatrix() * m_worldCamera.viewMatrix();
    auto transform = m_useDebugCamera ? debugTransform : worldTransform;

    m_lineRenderer.render(transform);
#endif  // 0
#endif

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

  ca::LineRenderer m_lineRenderer;

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

  ca::Ray m_ray;

  // Client size of the area we're rendering the world into:
  ca::Vec2 m_screenSize{0.0f, 0.0f};

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  ca::Vec2 m_currentMousePosition{0.0f, 0.0f};
};

CANVAS_APP(AsteroidDefender)
