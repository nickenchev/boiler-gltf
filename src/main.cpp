#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <rapidjson/document.h>

using namespace rapidjson;

namespace gltf
{
	struct Asset
	{
		std::string version, generator, copyright;
	};

	struct Node
	{
		std::vector<int> children;
	};

	struct Scene
	{
		std::vector<int> nodes;
	};

	struct Mesh
	{
	};

	struct Model
	{
		Asset asset;
		int scene;
		std::vector<Scene> scenes;
	};

	std::string getString(const Value &value, const std::string &key)
	{
		if (value.HasMember(key.c_str()))
		{
			return std::string(value[key.c_str()].GetString());
		}
		else
		{
			return "";
		}
	}

	Model load(const std::string &modelPath)
	{
		using namespace gltf;
		std::ifstream t(modelPath);
		std::stringstream buffer;
		buffer << t.rdbuf();

		Document document;
		document.Parse(buffer.str().c_str());

		using namespace gltf;
		Model model;

		// asset info
		assert(document.HasMember("asset") && document["asset"].IsObject());
		const Value &assetValue = document["asset"];
		model.asset.version = getString(assetValue, "version");
		model.asset.generator = getString(assetValue, "generator");
		model.asset.copyright = getString(assetValue, "copyright");

		// scene data
		if (document.HasMember("scene"))
		{
			model.scene = document["scene"].GetInt();

			assert(document["scenes"].IsArray());
			const Value &scenes = document["scenes"];
			model.scenes.reserve(scenes.Size());

			for (auto &scene : scenes.GetArray())
			{
				assert(scene.HasMember("nodes"));
				Scene newScene;
				for (auto &node : scene["nodes"].GetArray())
				{
					newScene.nodes.push_back(node.GetInt());
				}
				model.scenes.push_back(newScene);
			}
		}

		// nodes data

		return model;
	}
}

int main()
{
	auto model = gltf::load("models/Box.gltf");
	return 0;
}
