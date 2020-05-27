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
	using byte_size = unsigned int;
	struct Accessor
	{
		std::optional<int> bufferView;
		int byteOffset;
		int componentType;
		bool normalized;
		int count;
		std::string type;

		Accessor()
		{
			byteOffset = 0;
			normalized = false;
		}
	};

	struct BufferView
	{
		int buffer;
		std::optional<byte_size> byteOffset;
		std::optional<byte_size> byteLength;
		std::optional<byte_size> byteStride;
		std::optional<int> target;
		std::string name;
	};

	struct Buffer
	{
		std::string uri;
		byte_size byteLength;
		std::string name;

		Buffer(byte_size byteLength)
		{
			this->byteLength = byteLength;
		}
	};

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
		std::vector<Buffer> buffers;
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

	Model load(const std::string &&jsonData)
	{
		using namespace gltf;
		Document document;
		document.Parse(jsonData.c_str());

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
		const auto &meshes = document["meshes"].GetArray();
		model.meshes.reserve(meshes.Size());
		for (const auto &mesh : meshes)
		{
			assert(mesh.IsObject());
			Mesh newMesh;

			assert(mesh.HasMember("primitives"));
			const auto primitives = mesh["primitives"].GetArray();
			newMesh.primitives.reserve(primitives.Size());
			for (const auto& primitive : primitives)
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

		// accessors
		assert(document.HasMember("accessors"));
		const auto &accessors = document["accessors"].GetArray();

		// buffers
		assert(document.HasMember("buffers"));
		const auto &buffers = document["buffers"];
		model.buffers.reserve(buffers.GetArray().Size());
		for (const auto &buffer : buffers.GetArray())
		{
			Buffer newBuffer(buffer["byteLength"].GetInt());
			newBuffer.uri = getString(buffer, "uri");
			newBuffer.name = getString(buffer, "name");

			model.buffers.push_back(newBuffer);
		}

		return model;
	}
}

int main()
{
	std::ifstream t("models/Box.gltf");
	std::stringstream buffer;
	buffer << t.rdbuf();

	auto model = gltf::load(buffer.str());

	return 0;
}
