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

	for (const auto &buffer : model.buffers)
	{
		gltf::loadBuffer("models/", buffer);
	}

	gltf::ModelAccessors accessors(model);

	return 0;
}
