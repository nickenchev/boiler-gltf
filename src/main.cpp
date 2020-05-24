#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
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
		std::vector<float> matrix;
		int mesh;
	};

	struct Scene
	{
		std::vector<int> nodes;
	};

	struct Primitive
	{
		std::unordered_map<std::string, int> attributes;
		std::optional<int> indices;
		std::optional<int> material;
		std::optional<int> mode;
	};

	struct Mesh
	{
		std::vector<Primitive> primitives;
	};

	struct Model
	{
		Asset asset;
		int scene;
		std::vector<Scene> scenes;
		std::vector<Node> nodes;
		std::vector<Mesh> meshes;
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
	std::optional<int> getInt(const Value &value, const std::string &key)
	{
		if (value.HasMember(key.c_str()))
		{
			return std::optional<int>(value[key.c_str()].GetInt());
		}
		else
		{
			return std::optional<int>();
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

			for (const auto &scene : scenes.GetArray())
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
		if (document.HasMember("nodes"))
		{
			assert(document["nodes"].IsArray());
			model.nodes.reserve(document["nodes"].Size());

			for (const auto &node : document["nodes"].GetArray())
			{
				assert(node.IsObject());
				Node newNode;
				if (node.HasMember("children"))
				{
					for (const auto &child : node["children"].GetArray())
					{
						newNode.children.push_back(child.GetInt());
					}
				}

				if (node.HasMember("matrix"))
				{
					const auto &matrix = node["matrix"];
					assert(matrix.IsArray());

					newNode.matrix.reserve(matrix.Size());
					for (const auto &value : matrix.GetArray())
					{
						newNode.matrix.push_back(value.GetFloat());
					}
				}

				if (node.HasMember("mesh"))
				{
					newNode.mesh = node["mesh"].GetInt();
				}
				model.nodes.push_back(newNode);
			}
		}

		// meshes
		assert(document.HasMember("meshes"));
		for (const auto &mesh : document["meshes"].GetArray())
		{
			assert(mesh.IsObject());
			Mesh newMesh;

			assert(mesh.HasMember("primitives"));
			for (const auto& primitive : mesh["primitives"].GetArray())
			{
				Primitive newPrimitive;
				if (primitive.HasMember("attributes"))
				{
					const auto &attributes = primitive["attributes"];
					for (Value::ConstMemberIterator itr = attributes.MemberBegin();
						 itr != attributes.MemberEnd(); ++itr)
					{
						newPrimitive.attributes[itr->name.GetString()] = itr->value.GetInt();
					}
				}
				newPrimitive.indices = getInt(primitive, "indices");
				newPrimitive.mode = getInt(primitive, "mode");
				newPrimitive.material = getInt(primitive, "material");
				newMesh.primitives.push_back(newPrimitive);
			}
			model.meshes.push_back(newMesh);
		}

		return model;
	}
}

int main()
{
	auto model = gltf::load("models/Box.gltf");

	return 0;
}
