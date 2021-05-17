#include "ad/App/UserInterface.h"
#include "ad/World/ConstructionController.h"
#include "ad/World/Entity.h"
#include "ad/World/Generator.h"
#include "ad/World/Prefabs.h"
#include "ad/World/World.h"
#include "canvas/App.h"
#include "canvas/OpenGL.h"
#include "floats/Intersection.h"
#include "hive/physical_file_locator.h"
#include "legion/Controllers/TopDownCameraController.h"
#include "legion/Resources/ResourceManager.h"
#include "nucleus/FilePath.h"

class AsteroidDefender : public ca::WindowDelegate {
  NU_DELETE_COPY_AND_MOVE(AsteroidDefender);

public:
  AsteroidDefender() : ca::WindowDelegate{"Asteroid Defender"} {}

  ~AsteroidDefender() override = default;

  bool onWindowCreated(ca::Window* window) override {
    if (!WindowDelegate::onWindowCreated(window)) {
      return false;
    }

    ca::Renderer* renderer = window->getRenderer();

    auto assets_path = nu::getCurrentWorkingDirectory() / "assets";
    LOG(Info) << "Assets path: " << assets_path.getPath();
    auto locator = nu::makeScopedRefPtr<hi::PhysicalFileLocator>(assets_path);
    resource_manager_ = nu::makeScopedPtr<le::ResourceManager>(std::move(locator));

    prefabs_ = nu::makeScopedPtr<Prefabs>(resource_manager_.get());
    if (!create_prefabs(prefabs_.get())) {
      return false;
    }

    Generator::generate(&world_, prefabs_.get());

    world_camera_.moveTo({0.0f, 0.0f, 5.0f});
    world_camera_.setNearPlane(0.1f);
    world_camera_.setFarPlane(200.0f);

    construction_controller_ = nu::makeScopedPtr<ConstructionController>(&world_, prefabs_.get());

    user_interface_ =
        nu::makeScopedPtr<UserInterface>(construction_controller_.get(), world_.resources());
    if (!user_interface_->initialize(renderer)) {
      return false;
    }

    return true;
  }

  void onWindowResized(const fl::Size& size) override {
    WindowDelegate::onWindowResized(size);

    screen_size_ = size;

    auto aspect_ratio = le::Camera::aspectRatioFromScreenSize(screen_size_);

    world_camera_.setAspectRatio(aspect_ratio);

    //    user_interface_.ui().resize(size);
  }

  void on_mouse_moved(const ca::MouseEvent& event) override {
#if 0
    user_interface_.ui().on_mouse_moved(event);

    current_mouse_position_ = event.pos;

    current_camera_->onMouseMoved(
        le::Camera::convertScreenPositionToClipSpace(event.pos, screen_size_));
#endif  // 0
  }

  bool on_mouse_pressed(const ca::MouseEvent& event) override {
#if 0
    if (user_interface_.ui().on_mouse_pressed(event)) {
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

//    current_camera_->onMousePressed(
//        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, screen_size_));
#endif  // 0

    return false;
  }

  void on_mouse_released(const ca::MouseEvent& event) override {
#if 0
    user_interface_.ui().on_mouse_released(event);

    current_camera_->onMouseReleased(
        event.button, le::Camera::convertScreenPositionToClipSpace(event.pos, screen_size_));
#endif  // 0
  }

  void on_mouse_wheel(const ca::MouseWheelEvent& event) override {
#if 0
    user_interface_.ui().on_mouse_wheel(event);

    current_camera_->onMouseWheel(
        {static_cast<F32>(event.wheelOffset.x), static_cast<F32>(event.wheelOffset.y)});
#endif  // 0
  }

  void on_key_pressed(const ca::KeyEvent& event) override {
#if 0
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
#endif  // 0
  }

  void on_key_released(const ca::KeyEvent& event) override {
#if 0
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
#endif  // 0
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
      //      construction_controller_.set_cursor_position(cursor_position);
    }

    world_.tick(delta);
    //    user_interface_.tick(delta);
  }

  void onRender(ca::Renderer* renderer) override {
    renderer->state().depth_test(true);

    le::Camera* camera = current_camera_->camera();

    {
      PROFILE("render world")
      world_.render(renderer, camera);
    }

    //    construction_controller_.render(renderer, camera);

    glDisable(GL_DEPTH_TEST);

    {
        PROFILE("render user interface")
        //      user_interface_.ui().render(renderer);
    }

    renderer->state()
        .depth_test(false);
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
  static bool create_prefabs(Prefabs* prefabs) {
    if (!prefabs->set(EntityType::CommandCenter,
                      [](le::ResourceManager* resource_manager, Entity* storage) -> bool {
                        storage->electricity.electricity_delta = 20;
                        storage->building.selection_radius = 1.5f;

                        // auto model = resource_manager->get_model("command_center.dae");

                        //                        auto scene =
                        //                        resource_manager->get_scene("command_center.dae");
                        //                        auto model =
                        //                            le::Model::create_from_scene(*scene,
                        //                            resource_manager, renderer);
                        //
                        //                        storage->render.model = model;
                        //                        if (!storage->render.model) {
                        //                          return false;
                        //                        }

                        return true;
                      })) {
      return false;
    }

    if (!prefabs->set(EntityType::Miner,
                      [](le::ResourceManager* resource_manager, Entity* storage) -> bool {
                        return false;
#if 0
                        storage->electricity.electricity_delta = -5;

                        storage->building.selection_radius = 1.5f;

                        storage->mining.time_since_last_cycle = 0.0f;
                        storage->mining.cycle_duration = 100.0f;
                        storage->mining.mineral_amount_per_cycle = 10;

                        storage->render.model = resource_manager->get_scene("miner.dae").release();
                        if (!storage->render.model) {
                          return false;
                        }

                        return true;
#endif  // 0
                      })) {
      return false;
    }

    if (!prefabs->set(EntityType::Asteroid,
                      [](le::ResourceManager* resource_manager, Entity* storage) -> bool {
                        return false;
#if 0
                        // storage->render.model = resourceManager->get<le::Model>("asteroid.dae");
                        storage->render.model =
                            resource_manager->get_model("command_center.dae").release();
                        if (!storage->render.model) {
                          return false;
                        }

                        storage->building.selection_radius = 0.5f;

                        return true;
#endif  // 0
                      })) {
      return false;
    }

    if (!prefabs->set(EntityType::EnemyFighter,
                      [](le::ResourceManager* resource_manager, Entity* storage) -> bool {
                        return false;
#if 0
                        storage->render.model = resource_manager->get_model("enemy.dae").release();
                        if (!storage->render.model) {
                          return false;
                        }

                        return true;
#endif  // 0
                      })) {
      return false;
    }

    return true;
  }

  nu::ScopedPtr<le::ResourceManager> resource_manager_;

  nu::ScopedPtr<Prefabs> prefabs_;
  World world_;

  nu::ScopedPtr<ConstructionController> construction_controller_;

  nu::ScopedPtr<UserInterface> user_interface_;

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

#if !defined(AS_TESTS)
CANVAS_APP(AsteroidDefender)
#endif
