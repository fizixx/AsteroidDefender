#include "ad/Geometry/Geometry.h"
#include "ad/Sprites/SpriteConverter.h"
#include "ad/Sprites/SpriteRenderer.h"
#include "ad/World/CameraInputController.h"
#include "ad/World/World.h"
#include "canvas/App.h"
#include "canvas/Math/Intersection.h"
#include "canvas/Math/Transform.h"
#include "canvas/OpenGL.h"
#include "canvas/Renderer/LineRenderer.h"
#include "elastic/Context.h"
#include "elastic/Views/LabelView.h"
#include "hive/PhysicalResourceLocator.h"
#include "hive/ResourceManager.h"
#include "nucleus/Streams/FileInputStream.h"
#include "nucleus/Streams/WrappedMemoryInputStream.h"

void drawCross(ca::LineRenderer* lineRenderer, const ca::Vec3& position, const ca::Color& color,
               F32 scale = 1.0f) {
  lineRenderer->renderLine(position - ca::Vec3{1.0f, 0.0f, 0.0f} * scale,
                           position + ca::Vec3{1.0f, 0.0f, 0.0f} * scale, color);
  lineRenderer->renderLine(position - ca::Vec3{0.0f, 1.0f, 0.0f} * scale,
                           position + ca::Vec3{0.0f, 1.0f, 0.0f} * scale, color);
  lineRenderer->renderLine(position - ca::Vec3{0.0f, 0.0f, 1.0f} * scale,
                           position + ca::Vec3{0.0f, 0.0f, 1.0f} * scale, color);
}

const I8* kCubeVertexShaderSource = R"(
#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

uniform mat4 uTransform;

out vec4 vColor;

void main() {
  gl_Position = uTransform * vec4(inPosition, 1.0);
  vColor = inColor;
}
)";

const I8* kCubeFragmentShaderSource = R"(
#version 330

in vec4 vColor;
out vec4 final;

void main() {
  final = vec4(vColor.xyz, 1.0);
}
)";

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

    m_cube.geometry = createCube(renderer);
    if (!isValid(m_cube.geometry.vertexBufferId)) {
      return false;
    }

    nu::WrappedMemoryInputStream vertexStream{kCubeVertexShaderSource,
                                              nu::StringView{kCubeVertexShaderSource}.getLength()};
    auto vss = ca::ShaderSource::from(&vertexStream);

    nu::WrappedMemoryInputStream fragmentStream{
        kCubeFragmentShaderSource, nu::StringView{kCubeFragmentShaderSource}.getLength()};
    auto fss = ca::ShaderSource::from(&fragmentStream);

    m_cube.programId = renderer->createProgram(vss, fss);

    m_cube.transformUniformId = renderer->createUniform("uTransform");

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
    m_worldCamera.moveBy({0.0f, 0.0f, -evt.wheelOffset.y * 10.0f});
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

      // LOG(Info) << "origin = " << m_ray.origin << ", direction = " << m_ray.direction;

      ca::Plane worldPlane{{0.0f, 0.0f, 1.0f}, 0.0f};
      auto result = ca::intersection(worldPlane, m_ray);
      m_world.setCursorPosition({result.position.x, result.position.y});
    }

    m_world.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    m_lineRenderer.beginFrame();
    m_spriteRenderer.beginFrame(m_useDebugCamera ? &m_debugCamera : &m_worldCamera);

    drawCross(&m_lineRenderer, ca::Vec3::zero, ca::Color::red, 100.0f);

    m_world.render(&m_spriteRenderer);

    glEnable(GL_DEPTH_TEST);
    {
      static ca::Angle yRotation{ca::degrees(45.0f)};
      // yRotation += 1.0f;

      static ca::Angle yView = ca::degrees(0.0f);
      static F32 delta = 1.0f;
      yView += ca::degrees(delta);
      if (yView.degrees() > 100.0f || yView.degrees() < -100.0f) {
        delta *= -1.0f;
      }

      ca::UniformBuffer uniforms;

      ca::Mat4 projection = ca::perspectiveProjection(ca::degrees(120.0f), 1.0f, 0.1f, 50.0f);

      ca::Quaternion viewOrientation{
          ca::Quaternion::fromEulerAngles(ca::Angle::zero, yView, ca::Angle::zero)};
      ca::Mat4 view = ca::createViewMatrix({0.0f, 0.0f, 10.0f}, viewOrientation);

      {
        ca::Mat4 modelTranslation = ca::translationMatrix({0.5f, 0.0f, -2.0f});
        ca::Quaternion orientation =
            ca::Quaternion::fromEulerAngles(ca::degrees(0.0f), yRotation, ca::degrees(0.0f));
        ca::Mat4 modelRotation{orientation.toRotationMatrix()};
        ca::Mat4 modelScale = ca::scaleMatrix(0.5f);

        ca::Mat4 model = ca::createModelMatrix(modelTranslation, modelRotation, modelScale);

        ca::Mat4 transform = projection * view * model;

        uniforms.set(m_cube.transformUniformId, transform);
        renderGeometry(renderer, m_cube.geometry, m_cube.programId, uniforms);
      }

      {
        ca::Mat4 modelTranslation = ca::translationMatrix({-0.5f, 0.0f, -2.0f});
        ca::Mat4 modelRotation = ca::rotationMatrix(ca::Vec3::up, yRotation);
        ca::Mat4 modelScale = ca::scaleMatrix(0.5f);

        ca::Mat4 model = ca::createModelMatrix(modelTranslation, modelRotation, modelScale);

        ca::Mat4 transform = projection * view * model;

        uniforms.set(m_cube.transformUniformId, transform);
        renderGeometry(renderer, m_cube.geometry, m_cube.programId, uniforms);
      }
    }
    glDisable(GL_DEPTH_TEST);

    {
      ca::Mat4 projection = ca::Mat4::identity;
      ca::Mat4 view = ca::Mat4::identity;
      m_worldCamera.updateProjectionMatrix(&projection);
      m_worldCamera.updateViewMatrix(&view);
      m_lineRenderer.render(projection * view);
    }

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

  struct {
    Geometry geometry;
    ca::ProgramId programId;
    ca::UniformId transformUniformId;
  } m_cube;

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
