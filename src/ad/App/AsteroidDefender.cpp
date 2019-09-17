#include "ad/Geometry/Converters.h"
#include "ad/Geometry/Geometry.h"
#include "ad/Geometry/ShaderSourceConverter.h"
#include "ad/World/FirstPersonCameraController.h"
#include "ad/World/OrbitCameraController.h"
#include "ad/World/TopDownCameraController.h"
#include "ad/World/World.h"
#include "canvas/App.h"
#include "canvas/Math/Intersection.h"
#include "canvas/Math/Transform.h"
#include "canvas/OpenGL.h"
#include "canvas/Renderer/LineRenderer.h"
#include "elastic/Context.h"
#include "elastic/Views/ButtonView.h"
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

#if 0
    m_lineRenderer.initialize(renderer);
#endif

#if 0
    if (!createCube(&m_cube.model, renderer)) {
      LOG(Error) << "Could not create cube.";
      return false;
    }
#endif

    nu::WrappedMemoryInputStream vertexStream{kCubeVertexShaderSource,
                                              nu::StringView{kCubeVertexShaderSource}.getLength()};
    auto vss = ca::ShaderSource::from(&vertexStream);

    nu::WrappedMemoryInputStream fragmentStream{
        kCubeFragmentShaderSource, nu::StringView{kCubeFragmentShaderSource}.getLength()};
    auto fss = ca::ShaderSource::from(&fragmentStream);

    //    m_cube.programId = renderer->createProgram(vss, fss);

    //    m_cube.transformUniformId = renderer->createUniform("uTransform");

    auto assetsPath = nu::getCurrentWorkingDirectory() / "assets";
    LOG(Info) << "Assets path: " << assetsPath.getPath();
    m_physicalFileResourceLocator.setRootPath(assetsPath);
    m_resourceManager.addResourceLocatorBack(&m_physicalFileResourceLocator);

    m_converters.registerConverters(&m_resourceManager, renderer);

    m_model = m_resourceManager.get<Model>("command_center_lopoly.dae");
    if (!m_model) {
      LOG(Error) << "Could not load model.";
      return false;
    }

    if (!m_ui.initialize(renderer)) {
      return false;
    }

#if OS(POSIX)
    nu::FileInputStream fontStream{nu::FilePath {
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    }};
#elif OS(MACOSX)
    nu::FileInputStream fontStream{nu::FilePath { R"(/Library/Fonts/Arial.ttf)" }};
#else
    nu::FileInputStream fontStream{nu::FilePath{R"(C:\Windows\Fonts\Arial.ttf)"}};
#endif
    m_font.load(&fontStream, renderer, 20);

    if (!createUI(&m_ui, &m_font)) {
      return false;
    }

    if (!m_world.initialize(&m_resourceManager)) {
      LOG(Error) << "Could not initialize world.";
      return false;
    }

    m_world.generate();

    m_worldCamera.moveTo({0.0f, 0.0f, 5.0f});
    m_worldCamera.setNearPlane(0.1f);
    m_worldCamera.setFarPlane(200.0f);

    return true;
  }

  void onWindowResized(const ca::Size& size) override {
    WindowDelegate::onWindowResized(size);

    m_screenSize = size;

    auto aspectRatio = Camera::aspectRatioFromScreenSize(m_screenSize);

    m_worldCamera.setAspectRatio(aspectRatio);
    m_orbitCamera.setAspectRatio(aspectRatio);

    m_ui.resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& event) override {
    m_currentMousePosition = event.pos;

    m_currentCamera->onMouseMoved(
        Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
  }

  bool onMousePressed(const ca::MouseEvent& event) override {
    m_currentCamera->onMousePressed(
        event.button, Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));

    return false;
  }

  void onMouseReleased(const ca::MouseEvent& event) override {
    m_currentCamera->onMouseReleased(
        event.button, Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
  }

  void onMouseWheel(const ca::MouseWheelEvent& event) override {
    m_currentCamera->onMouseWheel(
        {static_cast<F32>(event.wheelOffset.x), static_cast<F32>(event.wheelOffset.y)});
  }

  void onKeyPressed(const ca::KeyEvent& event) override {
    m_currentCamera->onKeyPressed(event.key);

    switch (event.key) {
      case ca::Key::C:
        if (m_currentCamera == &m_worldCameraController) {
          m_cameraLabel->setLabel("orbit");
          m_currentCamera = &m_orbitCameraController;
        } else {
          m_cameraLabel->setLabel("world");
          m_currentCamera = &m_worldCameraController;
        }
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
    m_currentCamera->onKeyReleased(event.key);

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
    m_currentCamera->tick(delta);

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
#if 0
    m_lineRenderer.beginFrame();
#endif

#if 1
    glEnable(GL_DEPTH_TEST);

    // View (world)

    ca::Mat4 viewWorld{ca::Mat4::identity};
    m_worldCamera.updateViewMatrix(&viewWorld);

    // Final matrix

    ca::Mat4 finalMatrix = ca::Mat4::identity;
    ca::Mat4 camProjection = ca::Mat4::identity;
    ca::Mat4 camView = ca::Mat4::identity;

    Camera* current = m_currentCamera->camera();

    current->updateProjectionMatrix(&camProjection);
    current->updateViewMatrix(&camView);

    finalMatrix = camProjection * camView;
#endif  // 0

    // Render

    ca::Plane worldPlane{-ca::Vec3::forward, 0.0f};

    {
      ca::Ray mouseRay = m_worldCamera.createRayForMouse(
          Camera::convertScreenPositionToClipSpace(m_currentMousePosition, m_screenSize));

      auto intersection = ca::intersection(worldPlane, mouseRay);

      drawModel(renderer, intersection.position, ca::Quaternion::identity, finalMatrix);
    }

    Camera* camera = m_currentCamera->camera();

    m_world.render(renderer, camera);

    glDisable(GL_DEPTH_TEST);

    m_ui.render(renderer);
  }

  void drawModel(ca::Renderer* renderer, const ca::Vec3& position,
                 const ca::Quaternion& orientation, const ca::Mat4& finalMatrix) {
    ca::Mat4 modelTranslation = translationMatrix(position);
    ca::Mat4 modelRotation{orientation.toRotationMatrix()};
    ca::Mat4 modelScale = ca::scaleMatrix(0.5f);

    ca::Mat4 model = ca::createModelMatrix(modelTranslation, modelRotation, modelScale);

    ca::Mat4 final = finalMatrix * model;

    // renderModel(renderer, m_cube.model, final, m_cube.programId, m_cube.transformUniformId);

    renderModel(renderer, *m_model, final);
  }

  void drawCamera(Camera* camera) {
    // drawModel(renderer, camera->position(), camera->orientation(), finalMatrix);

#if 0
    m_lineRenderer.renderLine(camera->position(), camera->position() + camera->forward(),
                              ca::Color::red);
    m_lineRenderer.renderLine(camera->position(), camera->position() + camera->right(),
                              ca::Color::green);
    m_lineRenderer.renderLine(camera->position(), camera->position() + camera->up(),
                              ca::Color::blue);
#endif  // 0

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
      auto p = camInverse * v;
      p /= p.w;
      pos.emplaceBack(p);
    }

#if 0
    for (MemSize i = 0; i < 4; ++i) {
      m_lineRenderer.renderLine(pos[0 + i].xyz(), pos[0 + ((i + 1) % 4)].xyz(), ca::Color::white);
      m_lineRenderer.renderLine(pos[4 + i].xyz(), pos[4 + ((i + 1) % 4)].xyz(), ca::Color::white);
      m_lineRenderer.renderLine(pos[i].xyz(), pos[4 + i].xyz(), ca::Color::white);
    }
#endif
  }

private:
  DELETE_COPY_AND_MOVE(AsteroidDefender);

  struct BuildMinerClickListener : public el::ButtonView::OnClickListener {
    ~BuildMinerClickListener() override = default;

    void onButtonClicked(el::ButtonView *sender) override {
      LOG(Info) << "Build miner!";
    }
  };

  bool createUI(el::Context* context, el::Font* font) {
    el::ContextView* rootView = context->getRootView();

    m_cameraLabel = new el::LabelView{context, "world camera", font};
    rootView->addChild(m_cameraLabel);
    m_cameraLabel->setVerticalAlignment(el::Alignment::Top);
    m_cameraLabel->setHorizontalAlignment(el::Alignment::Left);

    m_buildMinerButton = new el::ButtonView{context, "Miner", new BuildMinerClickListener};
    rootView->addChild(m_buildMinerButton);

    return true;
  }

  hi::ResourceManager m_resourceManager;
  hi::PhysicalFileResourceLocator m_physicalFileResourceLocator;

  Model* m_model = nullptr;

  // ca::LineRenderer m_lineRenderer;

  el::Font m_font;
  el::Context m_ui;

  el::LabelView* m_cameraLabel = nullptr;
  el::ButtonView* m_buildMinerButton = nullptr;

  World m_world;

  F32 m_fieldOfViewMovement = 0.0f;

  Converters m_converters;

  Camera m_worldCamera{ca::degrees(45.0f), {0.0f, 0.0f, 1.0f}};
  TopDownCameraController m_worldCameraController{&m_worldCamera, {ca::Vec3::forward, 0.0f}, 25.0f};

  Camera m_orbitCamera{ca::degrees(45.0f), {0.0f, 0.0f, 1.0f}};
  OrbitCameraController m_orbitCameraController{&m_orbitCamera, ca::Vec3::zero};

  CameraController* m_currentCamera = &m_worldCameraController;

  ca::Ray m_ray;

  // Client size of the area we're rendering the world into:
  ca::Size m_screenSize;

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  ca::Pos m_currentMousePosition;
};

CANVAS_APP(AsteroidDefender)
