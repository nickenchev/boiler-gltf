#include "gltf.h"

namespace Boiler { namespace gltf
{
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
	
	std::optional<float> getFloat(const Value &value, const std::string &key)
	{
		if (value.HasMember(key.c_str()))
		{
			return std::optional<int>(value[key.c_str()].GetFloat());

		}
		else
		{
			return std::optional<float>();
		}
	}

	template<int Size>
	std::optional<std::array<float, Size>> getArray(const Value &value, const std::string &key)
	{
		std::optional<std::array<float, Size>> result;
		if (value.HasMember(key.c_str()))
		{
			const auto &array = value[key.c_str()];
			assert(array.IsArray());

			std::array<float, Size> floatArray;
			for (int i = 0; i < array.Size(); ++i)
			{
				floatArray[i] = array.GetArray()[i].GetFloat();
			}
			result = floatArray;
		}
		return result;
	}

	Model load(const std::string &jsonData)
	{
		using namespace gltf;
		Model model;

		Document document;
		document.Parse(jsonData.c_str());


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

				newNode.name = getString(node, "name");
				newNode.matrix = getArray<16>(node, "matrix");
				newNode.rotation = getArray<4>(node, "rotation");
				newNode.scale = getArray<3>(node, "scale");
				newNode.translation = getArray<3>(node, "translation");

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
			newMesh.name = getString(mesh, "name");

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
		model.accessors.reserve(accessors.Size());
		for (const auto &accessor : accessors)
		{
			Accessor newAccessor;
			newAccessor.bufferView = getInt(accessor, "bufferView");
			newAccessor.type = getString(accessor, "type");
			std::optional<int> byteOffset = getInt(accessor, "byteOffset");
			if (byteOffset.has_value())
			{
				newAccessor.byteOffset = byteOffset.value();
			}
			newAccessor.componentType = accessor["componentType"].GetInt();
			newAccessor.count = accessor["count"].GetInt();

			model.accessors.push_back(newAccessor);
		}

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

		// buffer views
		assert(document.HasMember("bufferViews"));
		const auto &bufferViews = document["bufferViews"].GetArray();
		model.bufferViews.reserve(bufferViews.Size());
		for (const auto &bufferView : bufferViews)
		{
			BufferView newBufferView;
			newBufferView.buffer = bufferView["buffer"].GetInt();
			auto byteOffset = getInt(bufferView, "byteOffset");
			if (byteOffset.has_value())
			{
				newBufferView.byteOffset = byteOffset.value();
			}
			newBufferView.byteLength = bufferView["byteLength"].GetInt();
			newBufferView.byteStride = getInt(bufferView, "byteStride");
			newBufferView.target = getInt(bufferView, "target");
			newBufferView.name = getString(bufferView, "name");

			model.bufferViews.push_back(newBufferView);
		}

		// materials
		if (document.HasMember("materials") && document["materials"].IsArray())
		{
			for (const auto &material : document["materials"].GetArray())
			{
				Material newMaterial;
				newMaterial.name = getString(material, "name");

				if (material.HasMember("pbrMetallicRoughness"))
				{
					auto getTexture = [](const Value &value, const std::string &key)
					{
						std::optional<MaterialTexture> matTexture;
						if (value.HasMember(key.c_str()))
						{
							const auto &material = value[key.c_str()];
							MaterialTexture newMatTexture;
							newMatTexture.index = getInt(material, "index");
							newMatTexture.texCoord = getInt(material, "texCoord");
							const auto scale = getFloat(material, "scale");
							if (scale.has_value()) newMatTexture.scale = scale.value();
						}
					};
					PBRMetallicRoughness pbrMetallicRoughness;
					pbrMetallicRoughness.baseColorFactor = getArray<4>(material["pbrMetallicRoughness"], "baseColorFactor");
				}
			}
		}

		return model;
	}

	std::vector<std::byte> loadBuffer(const std::string &basePath, const Buffer &buffer)
	{
		std::vector<std::byte> dataBuffer;
		dataBuffer.resize(buffer.byteLength);

		const std::string path = basePath + buffer.uri;

		std::ifstream ifs(path, std::ios::binary);
		if (ifs)
		{
			ifs.seekg(0);
			if(!ifs.read(reinterpret_cast<char *>(dataBuffer.data()), buffer.byteLength))
			{
				exit(1);
			}
			ifs.close();
		}

		return dataBuffer;
	}
};
};
