#ifndef GLTF_H
#define GLTF_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <unordered_map>
#include <rapidjson/document.h>

using namespace rapidjson;

namespace Boiler { namespace gltf
{
	namespace attributes
	{
		static inline const std::string POSITION = "POSITION";
	}

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
		byte_size byteOffset;
		std::optional<byte_size> byteLength;
		std::optional<byte_size> byteStride;
		std::optional<int> target;
		std::string name;

		BufferView()
		{
			byteOffset = 0;
		}
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

	struct Material
	{
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
		std::vector<BufferView> bufferViews;
		std::vector<Accessor> accessors;
	};

	std::string getString(const Value &value, const std::string &key);
	std::optional<int> getInt(const Value &value, const std::string &key);
	Model load(const std::string &jsonData);
	std::vector<std::byte> loadBuffer(const std::string &basePath, const Buffer &buffer);
};
};

#endif /* GLTF_H */
