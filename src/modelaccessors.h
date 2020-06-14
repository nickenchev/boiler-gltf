#ifndef MODELACCESSORS_H
#define MODELACCESSORS_H

#include "typedaccessor.h"

namespace Boiler { namespace gltf
{
	struct Model;

	class ModelAccessors
	{
		const Model &model;
		const std::vector<std::vector<std::byte>> buffers;

	public:
		ModelAccessors(const gltf::Model &model, std::vector<std::vector<std::byte>> &&buffers);

		template<typename ComponentType>
		TypedAccessor<ComponentType> getTypedAccessor(const Primitive &primitive, const std::string &attribute)
		{
			return getTypedAccessor<ComponentType>(model.accessors.at(primitive.attributes.find(attribute)->second));
		}

		template<typename ComponentType>
		TypedAccessor<ComponentType> getTypedAccessor(const Primitive &primitive, unsigned int accessorIndex)
		{
			return getTypedAccessor<ComponentType>(model.accessors.at(accessorIndex));
		}
		
		template<typename ComponentType>
		TypedAccessor<ComponentType> getTypedAccessor(const Accessor &accessor)
		{
			assert(accessor.bufferView.has_value());
			const BufferView &bufferView = model.bufferViews[accessor.bufferView.value()];
			const std::vector<std::byte> &data = buffers[bufferView.buffer];

			return TypedAccessor<ComponentType>(accessor, bufferView, data);
		}
	};
}
}

#endif /* MODELACCESSORS_H */
