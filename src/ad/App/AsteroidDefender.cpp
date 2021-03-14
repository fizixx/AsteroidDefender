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

    auto assets_path = nu::getCurrentWorkingDirectory() / "assets";
    LOG(Info) << "Assets path: " << assets_path.getPath();
    physical_file_resource_locator_.setRootPath(assets_path);
    resource_manager_.addResourceLocatorBack(&physical_file_resource_locator_);
    resource_manager_.setRenderer(renderer);

    create_prefabs(&prefabs_);

    Generator generator;
    generator.generate(&world_, &prefabs_);

    world_camera_.moveTo({0.0f, 0.0f, 5.0f});
    world_camera_.setNearPlane(0.1f);
    world_camera_.setFarPlane(200.0f);

    user_interface_.initialize(renderer);

    return true;
  }

  void onWindowResized(const fl::Size& size) override {
    WindowDelegate::onWindowResized(size);

    screen_size_ = size;

    auto aspect_ratio = le::Camera::aspectRatioFromScreenSize(screen_size_);

    world_camera_.setAspectRatio(aspect_ratio);

    user_interface_.ui().resize(size);
  }

  void onMouseMoved(const ca::MouseEvent& event) override {
    user_interface_.ui().onMouseMoved(event);

    current_mouse_position_ = event.pos;

    current_camera_->onMouseMoved(
        le::Camera::convertScreenPositionToClipSpace(event.pos, screen_size_));
  }

  bool onMousePressed(const ca::MouseEvent& event) override {
    if (user_interface_.ui().onMousePressed(event)) {
      return true;
    }

    if (construction_controller_.is_building() && event.button == ca::MouseEvent::Button::Left) {
      construction_controller_.build();
      return true;
    }

    if (event.button == ca::MouseEvent::Button::Left) {
      auto entity_under_cursor = world_.get_entity_under_cursor();
      if (entity_under_cursor.is_valid()) {
        LOG(Info) << "testing";
      }
    }

    current_camera_->onMousePressed(
        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, screen_size_));

    return false;
  }

  void onMouseReleased(const ca::MouseEvent& event) override {
    user_interface_.ui().onMouseReleased(event);

    current_camera_->onMouseReleased(
        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, screen_size_));
  }

  void onMouseWheel(const ca::MouseWheelEvent& event) override {
    user_interface_.ui().onMouseWheel(event);

    current_camera_->onMouseWheel(
        {static_cast<F32>(event.wheelOffset.x), static_cast<F32>(event.wheelOffset.y)});
  }

  void onKeyPressed(const ca::KeyEvent& event) override {
    current_camera_->onKeyPressed(event.key);

    switch (event.key) {
      case ca::Key::LBracket:
        field_of_view_movement_ -= 1.0f;
        break;

      case ca::Key::RBracket:
        field_of_view_movement_ += 1.0f;
        break;

      default:
        break;
    }
  }

  void onKeyReleased(const ca::KeyEvent& event) override {
    current_camera_->onKeyReleased(event.key);

    switch (event.key) {
      case ca::Key::LBracket:
        field_of_view_movement_ += 1.0f;
        break;

      case ca::Key::RBracket:
        field_of_view_movement_ -= 1.0f;
        break;

      default:
        break;
    }
  }

  void tick(F32 delta) override {
    current_camera_->tick(delta);

    {
      ray_ = world_camera_.createRayForMouse(
          le::Camera::convertScreenPositionToClipSpace(current_mouse_position_, screen_size_));

      fl::Plane world_plane{-fl::Vec3::forward, 0.0f};
      auto result = fl::intersection(world_plane, ray_);
      fl::Vec2 cursor_position{result.position.x, result.position.y};
      world_.set_cursor_position(cursor_position);
      construction_controller_.set_cursor_position(cursor_position);
    }

    world_.tick(delta);
    user_interface_.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    glEnable(GL_DEPTH_TEST);

    le::Camera* camera = current_camera_->camera();

    {
      PROFILE("render world")
      world_.render(renderer, camera);
    }

    construction_controller_.render(renderer, camera);

    glDisable(GL_DEPTH_TEST);

    {
      PROFILE("render user interface")
      user_interface_.ui().render(renderer);
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
  static auto create_prefabs(Prefabs* prefabs) -> void {
    prefabs->set(EntityType::CommandCenter,
                 [](hi::ResourceManager* resource_manager, Entity* storage) {
                   storage->electricity.electricity_delta = 20;
                   storage->building.selection_radius = 1.5f;
                   storage->render.model = resource_manager->get<le::Model>("command_center.dae");
                 });

    prefabs->set(EntityType::Miner, [](hi::ResourceManager* resource_manager, Entity* storage) {
      storage->electricity.electricity_delta = -5;

      storage->building.selection_radius = 1.5f;

      storage->mining.time_since_last_cycle = 0.0f;
      storage->mining.cycle_duration = 100.0f;
      storage->mining.mineral_amount_per_cycle = 10;

      storage->render.model = resource_manager->get<le::Model>("miner/miner.dae");
    });

    prefabs->set(EntityType::Asteroid, [](hi::ResourceManager* resource_manager, Entity* storage) {
      // storage->render.model = resourceManager->get<le::Model>("asteroid.dae");
      storage->render.model = resource_manager->get<le::Model>("command_center.dae");

      storage->building.selection_radius = 0.5f;
    });

    prefabs->set(EntityType::EnemyFighter,
                 [](hi::ResourceManager* resource_manager, Entity* storage) {
                   storage->render.model = resource_manager->get<le::Model>("enemy.dae");
                 });
  }

  le::ResourceManager resource_manager_;
  hi::PhysicalFileResourceLocator physical_file_resource_locator_;

  Prefabs prefabs_{&resource_manager_};
  World world_;

  ConstructionController construction_controller_{&world_, &prefabs_};

  UserInterface user_interface_{&construction_controller_, world_.resources()};

  F32 field_of_view_movement_ = 0.0f;

  le::Camera world_camera_{fl::degrees(45.0f), {0.0f, 0.0f, 1.0f}};
  le::TopDownCameraController world_camera_controller_{
      &world_camera_, {fl::Vec3::forward, 0.0f}, 25.0f};

  le::CameraController* current_camera_ = &world_camera_controller_;

  fl::Ray ray_{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};

  // Client size of the area we're rendering the world into:
  fl::Size screen_size_;

  // Position of the cursor on the screen in range ([0..m_size.width], [0..m_size.height]}.
  fl::Pos current_mouse_position_;
};

CANVAS_APP(AsteroidDefender)
