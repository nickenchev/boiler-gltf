#ifndef MODELACCESSORS_H
#define MODELACCESSORS_H

#include "gltf.h"
#include "typedaccessor.h"

namespace Boiler { namespace gltf
{
	struct Model;

	class ModelAccessors
	{
		const Model &model;
		const std::vector<std::vector<std::byte>> &buffers;

	public:
		ModelAccessors(const gltf::Model &model, const std::vector<std::vector<std::byte>> &buffers);

		template<typename ComponentType, unsigned short NumComponents>
		TypedAccessor<ComponentType, NumComponents> getTypedAccessor(const Primitive &primitive, const std::string &attribute) const
		{
			return getTypedAccessor<ComponentType, NumComponents>(model.accessors.at(primitive.attributes.find(attribute)->second));
		}

		template<typename ComponentType, unsigned short NumComponents>
		TypedAccessor<ComponentType, NumComponents> getTypedAccessor(unsigned int accessorIndex) const
		{
			return getTypedAccessor<ComponentType, NumComponents>(model.accessors.at(accessorIndex));
		}
		
		template<typename ComponentType, unsigned short NumComponents>
		TypedAccessor<ComponentType, NumComponents> getTypedAccessor(const Accessor &accessor) const
		{
			assert(accessor.bufferView.has_value());
			const BufferView &bufferView = model.bufferViews[accessor.bufferView.value()];

			if (bufferView.byteStride.has_value())
			{
				assert(bufferView.byteStride.value() == sizeof(ComponentType) * NumComponents);
			}

			const std::vector<std::byte> &data = buffers[bufferView.buffer];

			return TypedAccessor<ComponentType, NumComponents>(accessor, bufferView, data);
		}

		const std::byte *getPointer(const Accessor &accessor) const
		{
			const BufferView &bufferView = model.bufferViews[accessor.bufferView.value()];
			const std::vector<std::byte> &data = buffers[bufferView.buffer];
			return data.data() + (accessor.byteOffset + bufferView.byteOffset);
		}

		const Model &getModel() const { return model; }
	};
}
}

#endif /* MODELACCESSORS_H */
