#include <fstream>
#include <sstream>

#include "gltf.h"
#include "modelaccessors.h"

int main()
{
	// read the 
	std::ifstream t("models/Box.gltf");
	std::stringstream buffer;
	buffer << t.rdbuf();

	auto model = gltf::load(buffer.str());

	std::vector<std::vector<std::byte>> buffers;
	for (const auto &buffer : model.buffers)
	{
		buffers.push_back(loadBuffer("models/", buffer));
	}

	gltf::ModelAccessors modelAccess(model, std::move(buffers));
	for (auto &mesh : model.meshes)
	{
		for (auto &primitive : mesh.primitives)
		{
			using namespace gltf::attributes;

			// get the primitive's position data
			auto positionAccess = modelAccess.getTypedAccessor<float>(primitive, POSITION);
			for (auto values : positionAccess)
			{
				std::cout << values[0] << ", " << values[1] << ", " << values[2] << std::endl;
			}

			// check if we have indices for this primitive
			if (primitive.indices.has_value())
			{
				auto indexAccess = modelAccess.getTypedAccessor<unsigned short>(primitive, primitive.indices.value());
				for (auto index : indexAccess)
				{
					std::cout << index[0] << std::endl;
				}
			}
		}
	}

	return 0;
}
