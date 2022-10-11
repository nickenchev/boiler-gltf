#include <filesystem>
#include "gltf.h"

namespace Boiler { namespace gltf
{
	namespace keys
	{
		const static std::string NAME("name");
	};

	std::string getString(const Value &value, const std::string &key, const std::string &defaultValue) 
	{
		if (value.HasMember(key.c_str()))
		{
			return std::string(value[key.c_str()].GetString());
		}
		else
		{
			return defaultValue;
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
			return std::optional<float>(value[key.c_str()].GetFloat());

		}
		else
		{
			return std::optional<float>();
		}
	}

	void getBool(const Value &value, const std::string &key, bool &result)
	{
		if (value.HasMember(key.c_str()))
		{
			result = value[key.c_str()].GetBool();

		}
	}

	template<int Size>
	constexpr auto getArray(const Value &value, const std::string &key)
	{
		std::optional<std::array<float, Size>> result;
		if (value.HasMember(key.c_str()))
		{
			const auto &array = value[key.c_str()];
			assert(array.IsArray());

			std::array<float, Size> floatArray;
			for (unsigned int i = 0; i < array.Size(); ++i)
			{
				floatArray[i] = array.GetArray()[i].GetFloat();
			}
			result = floatArray;
		}
		return result;
	}

	template<int Size>
	std::array<float, Size> getArray(const Value &value, const std::string &key, std::array<float, Size> defaultValue)
	{
		std::optional<std::array<float, Size>> result = getArray<Size>(value, key);
		if (!result.has_value())
		{
			result = defaultValue;
		}

		return result.value();
	}

	auto getTexture(const Value &value, const std::string &key)
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

			matTexture = newMatTexture;
		}
		return matTexture;
	};

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

				newNode.name = getString(node, keys::NAME);
				newNode.matrix = getArray<16>(node, "matrix");
				newNode.translation = getArray<3>(node, "translation");
				newNode.rotation = getArray<4>(node, "rotation");
				newNode.scale = getArray<3>(node, "scale");

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
			newMesh.name = getString(mesh, keys::NAME);

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

			newAccessor.type = AccessorType::VEC3;
			std::string accessorType = getString(accessor, "type");
			if (accessorType == "SCALAR")
			{
				newAccessor.type = AccessorType::SCALAR;
			}
			else if (accessorType == "VEC2")
			{
				newAccessor.type = AccessorType::VEC2;
			}
			else if (accessorType == "VEC3")
			{
				newAccessor.type = AccessorType::VEC3;
			}
			else if (accessorType == "VEC4")
			{
				newAccessor.type = AccessorType::VEC4;
			}
			else if (accessorType == "MAT2")
			{
				newAccessor.type = AccessorType::MAT2;
			}
			else if (accessorType == "MAT3")
			{
				newAccessor.type = AccessorType::MAT3;
			}
			else if (accessorType == "MAT4")
			{
				newAccessor.type = AccessorType::MAT4;
			}

			std::optional<int> byteOffset = getInt(accessor, "byteOffset");
			if (byteOffset.has_value())
			{
				newAccessor.byteOffset = byteOffset.value();
			}
			newAccessor.componentType = static_cast<ComponentType>(accessor["componentType"].GetInt());
			newAccessor.count = accessor["count"].GetInt();
			newAccessor.name = getString(accessor, keys::NAME);

			if (accessor.HasMember("min"))
			{
				const auto &minValues = accessor["min"].GetArray();
				for (const auto &value : minValues)
				{
					newAccessor.min.push_back(AccessorValue(value.GetFloat()));
				}
			}
			if (accessor.HasMember("max"))
			{
				const auto &maxValues = accessor["max"].GetArray();
				for (const auto &value : maxValues)
				{
					newAccessor.max.push_back(AccessorValue(value.GetFloat()));
				}
			}

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
			newBuffer.name = getString(buffer, keys::NAME);

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
			newBufferView.name = getString(bufferView, keys::NAME);

			model.bufferViews.push_back(newBufferView);
		}

		// materials
		if (document.HasMember("materials") && document["materials"].IsArray())
		{
			const auto &materials = document["materials"].GetArray();
			model.materials.reserve(materials.Size());

			for (const auto &material : materials)
			{
				Material newMaterial;
				newMaterial.name = getString(material, keys::NAME);

				if (material.HasMember("pbrMetallicRoughness"))
				{
					const auto &pbrInfo = material["pbrMetallicRoughness"];

					newMaterial.pbrMetallicRoughness = PBRMetallicRoughness();
					PBRMetallicRoughness &pbrMetallicRoughness = newMaterial.pbrMetallicRoughness.value();

					// load PBR material info
					pbrMetallicRoughness.baseColorFactor = getArray<4>(pbrInfo, "baseColorFactor");
					pbrMetallicRoughness.baseColorTexture = getTexture(pbrInfo, "baseColorTexture");
					const auto metallicFactor = getFloat(pbrInfo, "metallicFactor");
					if (metallicFactor.has_value()) pbrMetallicRoughness.metallicFactor = metallicFactor.value();
					const auto roughnessFactor = getFloat(pbrInfo, "roughnessFactor");
					if (roughnessFactor.has_value()) pbrMetallicRoughness.roughnessFactor = roughnessFactor.value();
					pbrMetallicRoughness.metallicRoughnessTexture = getTexture(pbrInfo, "metallicRoughnessTexture");
				}
				newMaterial.normalTexture = getTexture(material, "normalTexture");
				newMaterial.occlusionTexture = getTexture(material, "occlusionTexture");
				newMaterial.emissiveTexture = getTexture(material, "emissiveTexture");
				newMaterial.emissiveFactor = getArray<3>(material, "emissiveFactor");
				newMaterial.alphaMode = getString(material, "alphaMode", "OPAQUE");
				const auto alphaCutoff = getFloat(material, "alphaCutoff");
				if (alphaCutoff.has_value()) newMaterial.alphaCutoff = alphaCutoff.value();
				getBool(material, "doubleSided", newMaterial.doubleSided);

				model.materials.push_back(newMaterial);
			}
		}

		// images
		if (document.HasMember("images"))
		{
			const auto &images = document["images"].GetArray();
			model.images.reserve(images.Size());

			for (const auto &image : images)
			{
				Image newImage;
				newImage.uri = getString(image, "uri");
				newImage.mimeType = getString(image, "mimeType");
				newImage.bufferView = getInt(image, "bufferView");
				newImage.name = getString(image, keys::NAME);

				model.images.push_back(newImage);
			}
		}

		// textures
		if (document.HasMember("textures"))
		{
			const auto &textures = document["textures"].GetArray();
			model.textures.reserve(textures.Size());

			for (const auto &texture : textures)
			{
				Texture newTexture;
				newTexture.name = getString(texture, keys::NAME);
				newTexture.sampler = getInt(texture, "sampler");
				newTexture.source = getInt(texture, "source");

				model.textures.push_back(newTexture);
			}
		}

		// animations
		if (document.HasMember("animations"))
		{
			const auto &animations = document["animations"].GetArray();
			model.animations.reserve(animations.Size());
			
			for (const auto &animation : animations)
			{
				Animation newAnimation;
				newAnimation.name = getString(animation, keys::NAME);

				// samplers
				if (animation.HasMember("samplers"))
				{
					const auto &samplers = animation["samplers"].GetArray();
					newAnimation.samplers.reserve(samplers.Size());

					for (const auto &sampler : samplers)
					{
						const std::string interpStr = getString(sampler, "interpolation", "LINEAR");
						Interpolation interpolation = Interpolation::LINEAR;
						if (interpStr == "STEP")
						{
							interpolation = Interpolation::STEP;
						}
						else if (interpStr == "CUBICSPLINE")
						{
							interpolation = Interpolation::CUBICSPLINE;
						}

						newAnimation.samplers.push_back(
							Sampler(sampler["input"].GetInt(), sampler["output"].GetInt(), interpolation));
					}
				}

				// channels
				if (animation.HasMember("channels"))
				{
					const auto &channels = animation["channels"].GetArray();
					newAnimation.channels.reserve(channels.Size());

					for (const auto &channel : channels)
					{
						const auto &target = channel["target"].GetObject();

						Target newTarget;
						if (target.HasMember("node"))
						{
							newTarget.node = target["node"].GetInt();
						}
						newTarget.path = target["path"].GetString();

						newAnimation.channels.push_back(
							Channel(channel["sampler"].GetInt(), newTarget));
					}
				}
				model.animations.push_back(newAnimation);
			}
		}

		return model;
	}

	std::vector<std::byte> loadBuffer(const std::string &basePath, const Buffer &buffer)
	{
		std::vector<std::byte> dataBuffer;
		dataBuffer.resize(buffer.byteLength);

		std::filesystem::path bufferPath(basePath);
		bufferPath.append(buffer.uri);

		std::ifstream ifs(bufferPath, std::ios::binary);
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
