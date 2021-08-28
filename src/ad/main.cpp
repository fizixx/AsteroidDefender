#include <legion/engine/engine.hpp>
#include <nucleus/main_header.hpp>

#include "context.hpp"
#include "ui_layer.hpp"
#include "world_layer.hpp"

#if !defined(AS_TESTS)
NU_MAIN_HEADER {
  nu::Profiling profiling;
  le::Engine engine;

  auto context = nu::make_scoped_ref_ptr<ad::Context>(&engine.resource_manager());

  engine.add_layer<ad::WorldLayer>(context);
  engine.add_layer<ad::UILayer>(context);

  return engine.run();
}
#endif
