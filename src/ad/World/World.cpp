#include "World.h"

#include "ad/Geometry/Geometry.h"
#include "ad/World/Camera.h"
#include "canvas/Math/Common.h"
#include "canvas/Math/Transform.h"
#include "hive/ResourceManager.h"

bool World::initialize(hi::ResourceManager* resourceManager) {
  if (!loadModels(resourceManager)) {
    LOG(Error) << "Could not load world models.";
    return false;
  }

  return true;
}

void World::generate() {
  m_entities.clear();

  createCommandCenter();

  createMiner(ca::Vec2{10.0f, 10.0f});
  createMiner(ca::Vec2{5.0f, 10.0f});
  createMiner(ca::Vec2{-5.0f, 5.0f});

  for (U32 i = 0; i < 50; ++i) {
    ca::Angle theta = ca::degrees((F32)(std::rand() % 360));
    F32 distance = (F32)(std::rand() % 100);
    createAsteroid(ca::Vec2{ca::cosine(theta) * distance, ca::sine(theta) * distance});
  }

  for (U32 i = 0; i < 50; ++i) {
    ca::Angle theta = ca::degrees((F32)(std::rand() % 360));
    F32 distance = (F32)(std::rand() % 100);
    createEnemy(ca::Vec2{ca::cosine(theta) * distance, ca::sine(theta) * distance});
  }
}

void World::setCursorPosition(const ca::Vec2& position) {
  m_cursorPosition = position;
}

void World::tick(F32 delta) {
  for (auto& entity : m_entities) {
    if (entity.movement.speed > 0.0f) {
      ca::Vec2 d{ca::cosine(entity.movement.direction), ca::sine(entity.movement.direction)};
      F32 distance = entity.movement.speed * 0.01f * delta;
      entity.position += d * distance;

      entity.movement.distanceTravelled += distance;

      if (entity.movement.distanceTravelled > 5.0f) {
        entity.movement.direction = ca::degrees((F32)(std::rand() % 360));
        entity.movement.distanceTravelled = 0.0f;
      }
    }
  }
}

bool World::loadModels(hi::ResourceManager* resourceManager) {
  m_models.commandCenter = resourceManager->get<re::Model>("command_center.dae");
  if (!m_models.commandCenter) {
    LOG(Error) << "Could not load command center model.";
    return false;
  }

  m_models.miner = resourceManager->get<re::Model>("miner/miner.dae");
  if (!m_models.miner) {
    LOG(Error) << "Could not load miner model.";
    return false;
  }

  m_models.asteroid = resourceManager->get<re::Model>("asteroid.dae");
  if (!m_models.asteroid) {
    LOG(Error) << "Could not load asteroid model.";
    return false;
  }

  m_models.enemy = resourceManager->get<re::Model>("enemy.dae");
  if (!m_models.enemy) {
    LOG(Error) << "Could not load enemy model.";
    return false;
  }

  return true;
}

EntityId World::createCommandCenter() {
  auto result = m_entities.emplaceBack();

  Entity& entity = result.element();

  entity.position = ca::Vec2::zero;
  entity.model = m_models.commandCenter;

  return EntityId{result.index()};
}

EntityId World::createMiner(const ca::Vec2& position) {
  auto result = m_entities.emplaceBack();

  Entity& entity = result.element();

  entity.position = position;
  entity.model = m_models.miner;

  return EntityId{result.index()};
}

EntityId World::createAsteroid(const ca::Vec2& position) {
  auto result = m_entities.emplaceBack();

  Entity& entity = result.element();

  entity.position = position;
  entity.model = m_models.asteroid;

  return EntityId{result.index()};
}

EntityId World::createEnemy(const ca::Vec2& position) {
  auto result = m_entities.emplaceBack();

  Entity& entity = result.element();

  entity.position = position;
  entity.model = m_models.enemy;

  entity.movement.direction = ca::degrees((F32)(std::rand() % 360));
  entity.movement.speed = 1.0f + (F32)(std::rand() % 2);

  return EntityId{result.index()};
}

void World::render(ca::Renderer* renderer, Camera* camera) {
  ca::Mat4 projection = ca::Mat4::identity;
  camera->updateProjectionMatrix(&projection);

  ca::Mat4 view = ca::Mat4::identity;
  camera->updateViewMatrix(&view);

  auto m = projection * view;

  for (auto& entity : m_entities) {
    auto translation = ca::translationMatrix(ca::Vec3{entity.position, 0.0f});
    auto rotation = ca::rotationMatrix(ca::Vec3{0.0f, 0.0f, 1.0f}, entity.movement.direction);

    auto final = m * ca::createModelMatrix(translation, rotation, ca::Mat4::identity);
    renderModel(renderer, *entity.model, final);
  }
}
