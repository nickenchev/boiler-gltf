#include "gltf.h"
#include "modelaccessors.h"

gltf::ModelAccessors::ModelAccessors(const gltf::Model &model,
									 std::vector<std::vector<std::byte>> &&buffers)
	: model(model), buffers(buffers)
{
}
