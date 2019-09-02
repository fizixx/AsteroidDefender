
#include "ad/Geometry/ShaderSourceConverter.h"

#include "hive/ResourceManager.h"
#include "nucleus/Streams/InputStream.h"

bool ShaderSourceConverter::load(hi::ResourceManager* UNUSED(resourceManager),
                                 nu::InputStream* inputStream, ca::ShaderSource* storage) {
  return storage->loadFromStream(inputStream);
}
