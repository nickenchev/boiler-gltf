#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
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
	};

	struct Scene
	{
		Scene();
	};

	struct Mesh
	{
		Mesh();
	};

	struct Model
	{
		Asset asset;
	};
}

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

int main()
{
  std::ifstream t("models/Box.gltf");
  std::stringstream buffer;
  buffer << t.rdbuf();

  Document document;
  document.Parse(buffer.str().c_str());


  using namespace gltf;
  Model model;

  // parse asset
  assert(document.HasMember("asset"));
  const Value &assetValue = document["asset"];
  model.asset.version = getString(assetValue, "version");
  model.asset.generator = getString(assetValue, "generator");
  model.asset.copyright = getString(assetValue, "copyright");

  return 0;
}
