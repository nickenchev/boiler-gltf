#include <fstream>
#include <sstream>

#include "gltf.h"
#include "modelaccessors.h"

int main()
{
	std::ifstream t("models/Box.gltf");
	std::stringstream buffer;
	buffer << t.rdbuf();

	auto model = gltf::load(buffer.str());

	std::vector<std::vector<std::byte>> buffers;
	for (const auto &buffer : model.buffers)
	{
		buffers.push_back(loadBuffer("models/", buffer));
	}

	gltf::ModelAccessors accessors(model, std::move(buffers));
	for (auto &mesh : model.meshes)
	{
		for (auto &primitive : mesh.primitives)
		{
			auto typedAccess = accessors.getTypedAccessor<float>(model.accessors[primitive.attributes["POSITION"]]);
			for (auto values : typedAccess)
			{
				std::cout << values[0] << ", " << values[1] << ", " << values[2] << std::endl;
			}
		}
	}

	return 0;
}
