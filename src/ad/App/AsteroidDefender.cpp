#include "ad/World/World.h"
#include "canvas/App.h"
#include "canvas/Math/Intersection.h"
#include "canvas/Math/Transform.h"
#include "canvas/OpenGL.h"
#include "elastic/Context.h"
#include "elastic/Views/ButtonView.h"
#include "elastic/Views/LabelView.h"
#include "elastic/Views/LinearSizerView.h"
#include "hive/PhysicalResourceLocator.h"
#include "legion/Controllers/FirstPersonCameraController.h"
#include "legion/Controllers/OrbitCameraController.h"
#include "legion/Controllers/TopDownCameraController.h"
#include "legion/Rendering/Rendering.h"
#include "legion/Resources/ResourceManager.h"
#include "nucleus/Streams/FileInputStream.h"

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

    auto assetsPath = nu::getCurrentWorkingDirectory() / "assets";
    LOG(Info) << "Assets path: " << assetsPath.getPath();
    m_physicalFileResourceLocator.setRootPath(assetsPath);
    m_resourceManager.addResourceLocatorBack(&m_physicalFileResourceLocator);
    m_resourceManager.setRenderer(renderer);

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
    m_font.load(&fontStream, renderer, 30);

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

    auto aspectRatio = le::Camera::aspectRatioFromScreenSize(m_screenSize);

    m_worldCamera.setAspectRatio(aspectRatio);
    m_orbitCamera.setAspectRatio(aspectRatio);

    m_ui.resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& event) override {
    m_ui.onMouseMoved(event);

    m_currentMousePosition = event.pos;

    m_currentCamera->onMouseMoved(
        le::Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
  }

  bool onMousePressed(const ca::MouseEvent& event) override {
    if (m_ui.onMousePressed(event)) {
      return true;
    }

    if (event.button == ca::MouseEvent::Button::Left) {
      m_world.build();
      return true;
    }

    m_currentCamera->onMousePressed(
        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));

    return false;
  }

  void onMouseReleased(const ca::MouseEvent& event) override {
    m_ui.onMouseReleased(event);

    m_currentCamera->onMouseReleased(
        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
  }

  void onMouseWheel(const ca::MouseWheelEvent& event) override {
    m_ui.onMouseWheel(event);

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
          le::Camera::convertScreenPositionToClipSpace(m_currentMousePosition, m_screenSize));

      ca::Plane worldPlane{-ca::Vec3::forward, 0.0f};
      auto result = ca::intersection(worldPlane, m_ray);
      m_world.setCursorPosition({result.position.x, result.position.y});
    }

    m_world.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    glEnable(GL_DEPTH_TEST);

    le::Camera* camera = m_currentCamera->camera();

    m_world.render(renderer, camera);

    glDisable(GL_DEPTH_TEST);

    m_ui.render(renderer);
  }

#if 0
  void drawModel(ca::Renderer* renderer, const ca::Vec3& position,
                 const ca::Quaternion& orientation, const ca::Mat4& finalMatrix) {
    ca::Mat4 modelTranslation = translationMatrix(position);
    ca::Mat4 modelRotation{orientation.toRotationMatrix()};
    ca::Mat4 modelScale = ca::scaleMatrix(0.5f);

    ca::Mat4 model = ca::createModelMatrix(modelTranslation, modelRotation, modelScale);

    ca::Mat4 final = finalMatrix * model;

    // renderModel(renderer, m_cube.model, final, m_cube.programId, m_cube.transformUniformId);

    le::renderModel(renderer, *m_model, final);
  }
#endif  // 0

#if 0
  void drawCamera(le::Camera* camera) {
    // drawModel(renderer, camera->position(), camera->orientation(), finalMatrix);

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
  }
#endif  // 0

private:
  DELETE_COPY_AND_MOVE(AsteroidDefender);

  bool createUI(el::Context* context, el::Font* font) {
    el::ContextView* rootView = context->getRootView();

    m_cameraLabel = new el::LabelView{context, "world camera", font};
    m_cameraLabel->setVerticalAlignment(el::Alignment::Top);
    m_cameraLabel->setHorizontalAlignment(el::Alignment::Right);
    rootView->addChild(m_cameraLabel);

    auto buttonContainer = new el::LinearSizerView{&m_ui};
    buttonContainer->setOrientation(el::Orientation::Vertical);
    buttonContainer->setHorizontalAlignment(el::Alignment::Left);
    buttonContainer->setVerticalAlignment(el::Alignment::Top);
    buttonContainer->setMinSize(ca::Size{250, 0});
    m_ui.getRootView()->addChild(buttonContainer);

    addBuildButton(buttonContainer, BuildingType::CommandCenter, "Command Center");
    addBuildButton(buttonContainer, BuildingType::Miner, "Miner");

    return true;
  }

  struct BuildClickListener : public el::ButtonView::OnClickListener {
    World* world;
    BuildingType buildingType;

    BuildClickListener(World* world, BuildingType buildingType)
      : world{world}, buildingType{buildingType} {}

    ~BuildClickListener() override = default;

    void onButtonClicked(el::ButtonView* sender) override {
      world->startBuilding(buildingType);
    }
  };

  auto addBuildButton(el::GroupView* container, BuildingType buildingType,
                      const nu::StringView& label) -> void {
    auto clickListener = new BuildClickListener{&m_world, buildingType};
    auto button = new el::ButtonView{&m_ui, label, clickListener};
    button->setFont(&m_font);
    button->setMinSize(ca::Size{0, 45});
    button->setExpansion(el::Expansion::Horizontal);
    container->addChild(button);
  }

  le::ResourceManager m_resourceManager;
  hi::PhysicalFileResourceLocator m_physicalFileResourceLocator;

  le::Model* m_model = nullptr;

  el::Font m_font;
  el::Context m_ui;

  el::LabelView* m_cameraLabel = nullptr;

  World m_world;

  F32 m_fieldOfViewMovement = 0.0f;

  le::Camera m_worldCamera{ca::degrees(45.0f), {0.0f, 0.0f, 1.0f}};
  le::TopDownCameraController m_worldCameraController{
      &m_worldCamera, {ca::Vec3::forward, 0.0f}, 25.0f};

  le::Camera m_orbitCamera{ca::degrees(45.0f), {0.0f, 0.0f, 1.0f}};
  le::OrbitCameraController m_orbitCameraController{&m_orbitCamera, ca::Vec3::zero};

  le::CameraController* m_currentCamera = &m_worldCameraController;

  ca::Ray m_ray;

  // Client size of the area we're rendering the world into:
  ca::Size m_screenSize;

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  ca::Pos m_currentMousePosition;
};

CANVAS_APP(AsteroidDefender)
