#include "ad/Geometry/Geometry.h"
#include "ad/Geometry/ModelConverter.h"
#include "ad/Geometry/ShaderSourceConverter.h"
#include "ad/Geometry/TextureConverter.h"
#include "ad/Sprites/SpriteConverter.h"
#include "ad/Sprites/SpriteRenderer.h"
#include "ad/World/CameraController.h"
#include "ad/World/TopDownCameraController.h"
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

    if (!createCube(&m_cube.model, renderer)) {
      LOG(Error) << "Could not create cube.";
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

    m_modelConverter.setRenderer(renderer);
    m_resourceManager.registerConverter(&m_modelConverter);

    m_textureConverter.setRenderer(renderer);
    m_resourceManager.registerConverter(&m_textureConverter);

    m_shaderSourceConverter.setRenderer(renderer);
    m_resourceManager.registerConverter(&m_shaderSourceConverter);

    m_model = m_resourceManager.get<Model>("command_center.dae");

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

    m_debugCamera.setFarPlane(5000.0f);

    m_worldCamera.moveTo({0.0f, 0.0f, 5.0f});
    m_worldCamera.setNearPlane(0.1f);
    m_worldCamera.setFarPlane(200.0f);

    return true;
  }

  void onWindowResized(const ca::Size& size) override {
    WindowDelegate::onWindowResized(size);

    m_screenSize = size;

    m_debugCamera.setAspectRatio(Camera::aspectRatioFromScreenSize(m_screenSize));
    m_worldCamera.setAspectRatio(Camera::aspectRatioFromScreenSize(m_screenSize));

    m_ui.resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& event) override {
    m_topDownCameraController.onMouseMoved(
        Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
    m_currentMousePosition = event.pos;
  }

  bool onMousePressed(const ca::MouseEvent& event) override {
    m_topDownCameraController.onMousePressed(
        event.button, Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));

    return false;
  }

  void onMouseReleased(const ca::MouseEvent& event) override {
    m_topDownCameraController.onMouseReleased(
        event.button, Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
  }

  void onMouseWheel(const ca::MouseWheelEvent& event) override {
    m_topDownCameraController.onMouseWheel(
        {static_cast<F32>(event.wheelOffset.x), static_cast<F32>(event.wheelOffset.y)});
  }

  void onKeyPressed(const ca::KeyEvent& event) override {
    m_topDownCameraController.onKeyPressed(event.key);

    switch (event.key) {
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

  void onKeyReleased(const ca::KeyEvent& event) override {
    m_topDownCameraController.onKeyReleased(event.key);

    switch (event.key) {
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
    m_topDownCameraController.tick(delta);

    {
      m_ray = m_worldCamera.createRayForMouse(
          Camera::convertScreenPositionToClipSpace(m_currentMousePosition, m_screenSize));

      ca::Plane worldPlane{-ca::Vec3::forward, 0.0f};
      auto result = ca::intersection(worldPlane, m_ray);
      m_world.setCursorPosition({result.position.x, result.position.y});
    }

    m_world.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    m_lineRenderer.beginFrame();

#if 1
    glEnable(GL_DEPTH_TEST);

    // View (world)

    ca::Mat4 viewWorld{ca::Mat4::identity};
    m_worldCamera.updateViewMatrix(&viewWorld);

    // Projection

    ca::Mat4 projection = ca::Mat4::identity;
    m_debugCamera.updateProjectionMatrix(&projection);

    // View (debug)

    m_debugCamera.moveTo({-25.0f, 25.0f, 50.0f});
    m_debugCamera.rotateTo(
        ca::Quaternion::fromEulerAngles(ca::degrees(-30.0f), ca::degrees(-30.0f), ca::Angle::zero));

    ca::Mat4 viewDebug{ca::Mat4::identity};
    m_debugCamera.updateViewMatrix(&viewDebug);

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
#if 0
    for (I32 z = -5; z <= 5; ++z) {
      for (I32 y = -5; y <= 5; ++y) {
        for (I32 x = -5; x <= 5; ++x) {
          const ca::Vec3& position =
              ca::Vec3{static_cast<F32>(x), static_cast<F32>(y), static_cast<F32>(z)};
          drawCube(renderer, position, ca::Quaternion::identity, finalMatrix);
        }
      }
    }
#endif  // 0

    if (m_useDebugCamera) {
      drawCamera(renderer, finalMatrix, &m_worldCamera);
    }

    ca::Plane worldPlane{-ca::Vec3::forward, 0.0f};

    {
      ca::Ray mouseRay = m_worldCamera.createRayForMouse(
          Camera::convertScreenPositionToClipSpace(m_currentMousePosition, m_screenSize));

      auto intersection = ca::intersection(worldPlane, mouseRay);

      drawCube(renderer, intersection.position, ca::Quaternion::identity, finalMatrix);

      ca::Vec3 p1 = mouseRay.origin;
      ca::Vec3 p2 = mouseRay.origin +
                    mouseRay.direction * (m_worldCamera.farPlane() + m_worldCamera.nearPlane());

      m_lineRenderer.renderLine(p1, p2, ca::Color::red);
      m_lineRenderer.renderLine(ca::Vec3::zero, p1, ca::Color::green);
      m_lineRenderer.renderLine(ca::Vec3::zero, p2, ca::Color::blue);
    }

    m_lineRenderer.renderGrid(worldPlane, ca::Vec3::up, ca::Color{1.0f, 1.0f, 1.0f, 0.1f}, 20,
                              1.0f);

    m_lineRenderer.render(finalMatrix);

    glDisable(GL_DEPTH_TEST);
#endif  // 0

    if (m_useDebugCamera) {
      m_spriteRenderer.beginFrame(&m_debugCamera);
    } else {
      m_spriteRenderer.beginFrame(&m_worldCamera);
    }
    m_world.render(&m_spriteRenderer);

    m_ui.render(renderer);
  }

  void drawCube(ca::Renderer* renderer, const ca::Vec3& position, const ca::Quaternion& orientation,
                const ca::Mat4& finalMatrix) {
    ca::Mat4 modelTranslation = translationMatrix(position);
    ca::Mat4 modelRotation{orientation.toRotationMatrix()};
    ca::Mat4 modelScale = ca::scaleMatrix(0.5f);

    ca::Mat4 model = ca::createModelMatrix(modelTranslation, modelRotation, modelScale);

    ca::Mat4 final = finalMatrix * model;

    // renderModel(renderer, m_cube.model, final, m_cube.programId, m_cube.transformUniformId);

    renderModel(renderer, *m_model, final, m_cube.programId, m_cube.transformUniformId);
  }

  void drawCamera(ca::Renderer* renderer, const ca::Mat4& finalMatrix, Camera* camera) {
    drawCube(renderer, camera->position(), camera->orientation(), finalMatrix);

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
#if 0
      pos.constructBack([&camInverse, &v](ca::Vec4* storage) {
        *storage = camInverse * v;
        *storage /= storage->w;
      });
#else
      auto p = camInverse * v;
      p /= p.w;
      pos.emplaceBack(p);
#endif
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
  ModelConverter m_modelConverter;
  TextureConverter m_textureConverter;
  ShaderSourceConverter m_shaderSourceConverter;

  Model* m_model = nullptr;

  ca::LineRenderer m_lineRenderer;

  el::Font m_font;
  el::Context m_ui;

  el::LabelView* m_cameraLabel = nullptr;

  SpriteRenderer m_spriteRenderer;
  World m_world;

  F32 m_fieldOfViewMovement = 0.0f;

  struct {
    Model model;
    ca::ProgramId programId;
    ca::UniformId transformUniformId;
  } m_cube;

  Camera m_worldCamera{ca::degrees(60.0f)};
  TopDownCameraController m_topDownCameraController{
      &m_worldCamera, {ca::Vec3::forward, 0.0f}, 100.0f};

  bool m_useDebugCamera = false;
  Camera m_debugCamera;

  ca::Ray m_ray;

  // Client size of the area we're rendering the world into:
  ca::Size m_screenSize;

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  ca::Pos m_currentMousePosition;
};

CANVAS_APP(AsteroidDefender)
