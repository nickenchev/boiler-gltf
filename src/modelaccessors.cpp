#include "gltf.h"
#include "modelaccessors.h"

using namespace Boiler::gltf;

ModelAccessors::ModelAccessors(const Model &model, const std::vector<std::vector<std::byte>> &buffers)
	: model(model), buffers(buffers)
{
}
