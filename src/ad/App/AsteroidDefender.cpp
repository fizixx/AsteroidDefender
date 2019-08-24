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

    m_worldCamera.moveTo({0.0f, 0.0f, 25.0f});
    m_worldCamera.setNearPlane(10.0f);
    m_worldCamera.setFarPlane(100.0f);

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
    m_worldCameraInputController.onMouseMoved(event);
    m_currentMousePosition = {static_cast<F32>(event.pos.x), static_cast<F32>(event.pos.y)};
  }

  bool onMousePressed(const ca::MouseEvent& event) override {
    m_worldCameraInputController.onMousePressed(event);

    return false;
  }

  void onMouseReleased(const ca::MouseEvent& event) override {
    m_worldCameraInputController.onMouseReleased(event);
  }

  void onMouseWheel(const ca::MouseWheelEvent& evt) override {
    m_worldCamera.setFieldOfView(m_worldCamera.fieldOfView() +
                                 ca::degrees(-static_cast<F32>(evt.wheelOffset.y)));
  }

  void onKeyPressed(const ca::KeyEvent& evt) override {
    m_worldCameraInputController.onKeyPressed(evt);

    switch (evt.key) {
      case ca::Key::C:
        m_useDebugCamera = !m_useDebugCamera;
        m_cameraLabel->setLabel(m_useDebugCamera ? "debug" : "world");
        break;

      case ca::Key::LBracket:
        m_fieldOfViewMovement -= 1.0f;
        break;

      case ca::Key::RBracket:
        m_fieldOfViewMovement += 1.0f;
        break;

      default:
        break;
    }
  }

  void onKeyReleased(const ca::KeyEvent& evt) override {
    m_worldCameraInputController.onKeyReleased(evt);

    switch (evt.key) {
      case ca::Key::LBracket:
        m_fieldOfViewMovement += 1.0f;
        break;

      case ca::Key::RBracket:
        m_fieldOfViewMovement -= 1.0f;
        break;

      default:
        break;
    }
  }

  void tick(F32 delta) override {
    m_worldCameraInputController.tick(delta);
    m_worldCamera.setFieldOfView(m_worldCamera.fieldOfView() +
                                 ca::degrees(m_fieldOfViewMovement * delta * 0.1f));

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
    m_spriteRenderer.beginFrame(&m_worldCamera);

    glEnable(GL_DEPTH_TEST);

    // View (world)

    ca::Mat4 viewWorld{ca::Mat4::identity};
    m_worldCamera.updateViewMatrix(&viewWorld);

    // Projection

    ca::Mat4 projection = ca::perspectiveProjection(ca::degrees(45.0f), 1.0f, 0.1f, 15000.0f);

    // View (debug)

    Camera debugCamera;
    debugCamera.moveTo({-25.0f, 25.0f, 50.0f});
    debugCamera.rotateTo(
        ca::Quaternion::fromEulerAngles(ca::degrees(-30.0f), ca::degrees(-30.0f), ca::Angle::zero));

    ca::Mat4 viewDebug{ca::Mat4::identity};
    debugCamera.updateViewMatrix(&viewDebug);

    // Final matrix

    ca::Mat4 finalMatrix = ca::Mat4::identity;
    if (m_useDebugCamera) {
      finalMatrix = projection * viewDebug;
    } else {
      ca::Mat4 camProjection = ca::Mat4::identity;
      ca::Mat4 camView = ca::Mat4::identity;

      m_worldCamera.updateProjectionMatrix(&camProjection);
      m_worldCamera.updateViewMatrix(&camView);

      finalMatrix = camProjection * camView;
    }

    // Render
    for (I32 z = -5; z <= 5; ++z) {
      for (I32 y = -5; y <= 5; ++y) {
        for (I32 x = -5; x <= 5; ++x) {
          const ca::Vec3& position =
              ca::Vec3{static_cast<F32>(x), static_cast<F32>(y), static_cast<F32>(z)};
          drawCube(renderer, position, ca::Quaternion::identity, finalMatrix);
        }
      }
    }

    if (m_useDebugCamera) {
      drawCamera(renderer, finalMatrix, &m_worldCamera);
    }

    {
      ca::Ray mouseRay = m_worldCamera.createRayForMouse(m_currentMousePosition);

      ca::Plane p{-ca::Vec3::forward, 0.0f};
      auto intersection = ca::intersection(p, mouseRay);

      drawCube(renderer, intersection.position, ca::Quaternion::identity, finalMatrix);

      ca::Vec3 p1 = mouseRay.origin;
      ca::Vec3 p2 = mouseRay.origin +
                    mouseRay.direction * (m_worldCamera.farPlane() + m_worldCamera.nearPlane());

      m_lineRenderer.renderLine(p1, p2, ca::Color::red);
      m_lineRenderer.renderLine(ca::Vec3::zero, p1, ca::Color::green);
      m_lineRenderer.renderLine(ca::Vec3::zero, p2, ca::Color::blue);
    }

    m_lineRenderer.render(finalMatrix);

    glDisable(GL_DEPTH_TEST);

    m_ui.render(renderer);
  }

  void drawCube(ca::Renderer* renderer, const ca::Vec3& position, const ca::Quaternion& orientation,
                const ca::Mat4& finalMatrix) {
    ca::UniformBuffer uniforms;

    ca::Mat4 modelTranslation = translationMatrix(position);
    ca::Mat4 modelRotation{orientation.toRotationMatrix()};
    ca::Mat4 modelScale = ca::scaleMatrix(0.5f);

    ca::Mat4 model = ca::createModelMatrix(modelTranslation, modelRotation, modelScale);

    ca::Mat4 final = finalMatrix * model;

    uniforms.set(m_cube.transformUniformId, final);
    renderGeometry(renderer, m_cube.geometry, m_cube.programId, uniforms);
  }

  void drawCamera(ca::Renderer* renderer, const ca::Mat4& finalMatrix, Camera* camera) {
    drawCube(renderer, m_worldCamera.position(), camera->orientation(), finalMatrix);

    m_lineRenderer.renderLine(camera->position(), camera->position() + camera->forward(),
                              ca::Color::red);
    m_lineRenderer.renderLine(camera->position(), camera->position() + camera->right(),
                              ca::Color::green);
    m_lineRenderer.renderLine(camera->position(), camera->position() + camera->up(),
                              ca::Color::blue);

    // Draw the frustum.
    ca::Mat4 camProjection = ca::Mat4::identity;
    ca::Mat4 camView = ca::Mat4::identity;

    camera->updateProjectionMatrix(&camProjection);
    camera->updateViewMatrix(&camView);

    ca::Mat4 camInverse{ca::inverse(camProjection * camView)};

    static ca::Vec4 vertices[] = {
        {-1.0f, -1.0f, -1.0f, 1.0f},  //
        {+1.0f, -1.0f, -1.0f, 1.0f},  //
        {+1.0f, +1.0f, -1.0f, 1.0f},  //
        {-1.0f, +1.0f, -1.0f, 1.0f},  //
        {-1.0f, -1.0f, +1.0f, 1.0f},  //
        {+1.0f, -1.0f, +1.0f, 1.0f},  //
        {+1.0f, +1.0f, +1.0f, 1.0f},  //
        {-1.0f, +1.0f, +1.0f, 1.0f},  //
    };

    nu::DynamicArray<ca::Vec4> pos;
    for (ca::Vec4& v : vertices) {
      pos.pushBack([&camInverse, &v](ca::Vec4* storage) {
        *storage = camInverse * v;
        *storage /= storage->w;
      });
    }

    for (MemSize i = 0; i < 4; ++i) {
      m_lineRenderer.renderLine(pos[0 + i].xyz(), pos[0 + ((i + 1) % 4)].xyz(), ca::Color::white);
      m_lineRenderer.renderLine(pos[4 + i].xyz(), pos[4 + ((i + 1) % 4)].xyz(), ca::Color::white);
      m_lineRenderer.renderLine(pos[i].xyz(), pos[4 + i].xyz(), ca::Color::white);
    }
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
  World m_world;

  F32 m_fieldOfViewMovement = 0.0f;

  struct {
    Geometry geometry;
    ca::ProgramId programId;
    ca::UniformId transformUniformId;
  } m_cube;

  Camera m_worldCamera{ca::degrees(60.0f)};
  CameraInputController m_worldCameraInputController{&m_worldCamera, 0.1f};

  bool m_useDebugCamera = true;

  ca::Ray m_ray;

  // Client size of the area we're rendering the world into:
  ca::Vec2 m_screenSize{0.0f, 0.0f};

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  ca::Vec2 m_currentMousePosition{0.0f, 0.0f};
};

CANVAS_APP(AsteroidDefender)
