#include "ad/App/UserInterface.h"
#include "ad/World/ConstructionController.h"
#include "ad/World/Entity.h"
#include "ad/World/Generator.h"
#include "ad/World/Prefabs.h"
#include "ad/World/World.h"
#include "canvas/App.h"
#include "canvas/OpenGL.h"
#include "floats/Intersection.h"
#include "hive/PhysicalResourceLocator.h"
#include "legion/Controllers/TopDownCameraController.h"
#include "legion/Resources/ResourceManager.h"
#include "nucleus/FilePath.h"

class AsteroidDefender : public ca::WindowDelegate {
public:
  NU_DELETE_COPY_AND_MOVE(AsteroidDefender);

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
    m_resourceManager.setRenderer(renderer);

    createPrefabs(&m_prefabs);

    Generator generator;
    generator.generate(&m_world, &m_prefabs);

    m_worldCamera.moveTo({0.0f, 0.0f, 5.0f});
    m_worldCamera.setNearPlane(0.1f);
    m_worldCamera.setFarPlane(200.0f);

    m_userInterface.initialize(renderer);

    return true;
  }

  void onWindowResized(const fl::Size& size) override {
    WindowDelegate::onWindowResized(size);

    m_screenSize = size;

    auto aspectRatio = le::Camera::aspectRatioFromScreenSize(m_screenSize);

    m_worldCamera.setAspectRatio(aspectRatio);

    m_userInterface.ui().resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& event) override {
    m_userInterface.ui().onMouseMoved(event);

    m_currentMousePosition = event.pos;

    m_currentCamera->onMouseMoved(
        le::Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
  }

  bool onMousePressed(const ca::MouseEvent& event) override {
    if (m_userInterface.ui().onMousePressed(event)) {
      return true;
    }

    if (m_constructionController.isBuilding() && event.button == ca::MouseEvent::Button::Left) {
      m_constructionController.build();
      return true;
    }

    if (event.button == ca::MouseEvent::Button::Left) {
      auto entityUnderCursor = m_world.getEntityUnderCursor();
      if (entityUnderCursor.isValid()) {
        LOG(Info) << "testing";
      }
    }

    m_currentCamera->onMousePressed(
        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));

    return false;
  }

  void onMouseReleased(const ca::MouseEvent& event) override {
    m_userInterface.ui().onMouseReleased(event);

    m_currentCamera->onMouseReleased(
        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, m_screenSize));
  }

  void onMouseWheel(const ca::MouseWheelEvent& event) override {
    m_userInterface.ui().onMouseWheel(event);

    m_currentCamera->onMouseWheel(
        {static_cast<F32>(event.wheelOffset.x), static_cast<F32>(event.wheelOffset.y)});
  }

  void onKeyPressed(const ca::KeyEvent& event) override {
    m_currentCamera->onKeyPressed(event.key);

    switch (event.key) {
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

      fl::Plane worldPlane{-fl::Vec3::forward, 0.0f};
      auto result = fl::intersection(worldPlane, m_ray);
      fl::Vec2 cursorPosition{result.position.x, result.position.y};
      m_world.setCursorPosition(cursorPosition);
      m_constructionController.setCursorPosition(cursorPosition);
    }

    m_world.tick(delta);
    m_userInterface.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    glEnable(GL_DEPTH_TEST);

    le::Camera* camera = m_currentCamera->camera();

    {
      PROFILE("render world")
      m_world.render(renderer, camera);
    }

    m_constructionController.render(renderer, camera);

    glDisable(GL_DEPTH_TEST);

    {
      PROFILE("render user interface")
      m_userInterface.ui().render(renderer);
    }
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
  static auto createPrefabs(Prefabs* prefabs) -> void {
    prefabs->set(EntityType::CommandCenter,
                 [](hi::ResourceManager* resourceManager, Entity* storage) {
                   storage->electricity.electricityDelta = 20;
                   storage->building.selectionRadius = 1.5f;
                   storage->render.model = resourceManager->get<le::Model>("command_center.dae");
                 });

    prefabs->set(EntityType::Miner, [](hi::ResourceManager* resourceManager, Entity* storage) {
      storage->electricity.electricityDelta = -5;

      storage->building.selectionRadius = 1.5f;

      storage->mining.timeSinceLastCycle = 0.0f;
      storage->mining.cycleDuration = 100.0f;
      storage->mining.mineralAmountPerCycle = 10;

      storage->render.model = resourceManager->get<le::Model>("miner/miner.dae");
    });

    prefabs->set(EntityType::Asteroid, [](hi::ResourceManager* resourceManager, Entity* storage) {
      // storage->render.model = resourceManager->get<le::Model>("asteroid.dae");
      storage->render.model = resourceManager->get<le::Model>("command_center.dae");

      storage->building.selectionRadius = 0.5f;
    });

    prefabs->set(EntityType::EnemyFighter,
                 [](hi::ResourceManager* resourceManager, Entity* storage) {
                   storage->render.model = resourceManager->get<le::Model>("enemy.dae");
                 });
  }

  le::ResourceManager m_resourceManager;
  hi::PhysicalFileResourceLocator m_physicalFileResourceLocator;

  Prefabs m_prefabs{&m_resourceManager};
  World m_world;

  ConstructionController m_constructionController{&m_world, &m_prefabs};

  UserInterface m_userInterface{&m_constructionController, m_world.resources()};

  F32 m_fieldOfViewMovement = 0.0f;

  le::Camera m_worldCamera{fl::degrees(45.0f), {0.0f, 0.0f, 1.0f}};
  le::TopDownCameraController m_worldCameraController{
      &m_worldCamera, {fl::Vec3::forward, 0.0f}, 25.0f};

  le::CameraController* m_currentCamera = &m_worldCameraController;

  fl::Ray m_ray{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  // Client size of the area we're rendering the world into:
  fl::Size m_screenSize;

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  fl::Pos m_currentMousePosition;
};

CANVAS_APP(AsteroidDefender)
