#ifndef MODELACCESSORS_H
#define MODELACCESSORS_H

#include "typedaccessor.h"

namespace gltf
{
	struct Model;

	class ModelAccessors
	{
		const Model &model;
	public:
		ModelAccessors(const Model &model);

		template<unsigned int NumComponents, typename ComponentType>
		TypedAccessor<NumComponents, ComponentType> getTypedAccessor(const Accessor &accessor)
		{
			assert(accessor.bufferView.has_value());
			//const BufferView &bufferView = model.bu
		}
	};
};

#endif /* MODELACCESSORS_H */
