#ifndef GLTF_H
#define GLTF_H

#include <array>
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

using floatArray3 = std::array<float, 3>;
using floatArray4 = std::array<float, 4>;
using floatArray16 = std::array<float, 16>;

namespace ComponentTypes
{

template<typename Type, unsigned short ComponentCount>
struct ComponentType
{
	static constexpr Type type;
    static constexpr unsigned short componentCount = ComponentCount;
};

struct VEC3 : ComponentType<float, 3> {};

}

namespace attributes
{
    static inline const std::string POSITION = "POSITION";
    static inline const std::string NORMAL = "NORMAL";
    static inline const std::string TEXCOORD_0 = "TEXCOORD_0";
}

struct GLTFBase
{
};

using byte_size = unsigned int;

union AccessorValue
{
    char asByte;
    unsigned char asUnsignedByte;
    short asShort;
    unsigned short asUnsignedShort;
    unsigned int asUnsignedInt;
    float asFloat;

    AccessorValue(float value)
    {
        this->asFloat = value;
    }
};

enum class ComponentType
{
    BYTE = 5120,
    UNSIGNED_BYTE= 5121,
    SHORT = 5122,
    UNSIGNED_SHORT = 5123,
    UNSIGNED_INT = 5125,
    FLOAT = 5126,
};

enum class Interpolation
{
    LINEAR,
    STEP,
    CUBICSPLINE
};

struct Accessor : GLTFBase
{
    std::optional<byte_size> bufferView;
    byte_size byteOffset;
    ComponentType componentType;
    bool normalized;
    unsigned int count;
    std::string type;
    std::vector<AccessorValue> max, min;
    std::string name;

    Accessor()
    {
        byteOffset = 0;
        normalized = false;
    }
};

struct BufferView : GLTFBase
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

struct Buffer : GLTFBase
{
    std::string uri;
    byte_size byteLength;
    std::string name;

    Buffer(byte_size byteLength)
    {
        this->byteLength = byteLength;
    }
};

struct Asset : GLTFBase
{
    std::string version, generator, copyright;
};

struct Node : GLTFBase
{
    std::vector<int> children;
    std::optional<floatArray16> matrix;
    std::optional<int> mesh;
    std::optional<floatArray4> rotation;
    std::optional<floatArray3> scale;
    std::optional<floatArray3> translation;
    std::string name;
};

struct Scene : GLTFBase
{
    std::vector<int> nodes;
};

struct Primitive : GLTFBase
{
    std::unordered_map<std::string, int> attributes;
    std::optional<int> indices;
    std::optional<int> material;
    std::optional<int> mode;
};

struct Mesh : GLTFBase
{
    std::string name;
    std::vector<Primitive> primitives;
};

struct Image : GLTFBase
{
    std::string uri;
    std::string mimeType;
    std::optional<int> bufferView;
    std::string name;
};

struct Texture : GLTFBase
{
    std::optional<int> sampler;
    std::optional<int> source;
    std::string name;
};

struct MaterialTexture : GLTFBase
{
    std::optional<unsigned int> index;
    std::optional<unsigned int> texCoord;
    float scale;

    MaterialTexture()
    {
        scale = 1.0f;
    }
};

struct PBRMetallicRoughness : GLTFBase
{
    std::optional<floatArray4> baseColorFactor;
    std::optional<MaterialTexture> baseColorTexture;
    float metallicFactor;
    float roughnessFactor;
    std::optional<MaterialTexture> metallicRoughnessTexture;

    PBRMetallicRoughness()
    {
        baseColorFactor = floatArray4{1, 1, 1, 1};
        metallicFactor = 1;
        roughnessFactor = 1;
    }
};

struct Material : GLTFBase
{
    std::string name;
    std::optional<PBRMetallicRoughness> pbrMetallicRoughness;
    std::optional<MaterialTexture> normalTexture;
    std::optional<MaterialTexture> occlusionTexture;
    std::optional<MaterialTexture> emissiveTexture;
    std::optional<floatArray3> emissiveFactor;
    std::string alphaMode;
    float alphaCutoff;
    bool doubleSided;

    Material()
    {
        emissiveFactor = {0, 0, 0};
        alphaMode = "OPAQUE";
        alphaCutoff = 0.5f;
        doubleSided = false;
    }
};

struct Target : GLTFBase
{
    std::optional<unsigned int>node;
    std::string path;
};

struct Sampler : GLTFBase
{
    unsigned int input, output;
    Interpolation interpolation;

    Sampler(unsigned int input, unsigned int output,
            Interpolation interpolation)
    {
        this->input = input;
        this->output = output;
        this->interpolation = interpolation;
    }
};

struct Channel : GLTFBase
{
    unsigned int sampler;
	const Target target;

    Channel(unsigned int sampler, Target target) : target(target)
    {
		this->sampler = sampler;
    }
};

struct Animation : GLTFBase
{
    std::string name;
    std::vector<Channel> channels;
    std::vector<Sampler> samplers;
};

struct Model : GLTFBase
{
    Asset asset;
    int scene;
    std::vector<Scene> scenes;
    std::vector<Node> nodes;
    std::vector<Mesh> meshes;
    std::vector<Buffer> buffers;
    std::vector<BufferView> bufferViews;
    std::vector<Accessor> accessors;
    std::vector<Material> materials;
    std::vector<Image> images;
    std::vector<Texture> textures;
    std::vector<Animation> animations;
};

std::string getString(const Value &value, const std::string &key, const std::string &defaultValue = "");
std::optional<int> getInt(const Value &value, const std::string &key);
Model load(const std::string &jsonData);
std::vector<std::byte> loadBuffer(const std::string &basePath, const Buffer &buffer);
};
};

#endif /* GLTF_H */
