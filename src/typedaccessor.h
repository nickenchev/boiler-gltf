#ifndef TYPEDACCESSOR_H
#define TYPEDACCESSOR_H

#include "gltf.h"

namespace gltf
{
	template<unsigned int NumComponents, typename ComponentType>
	class TypedAccessor
	{
		static const unsigned int numComponents = NumComponents;
		const Accessor &accessor;

	public:
		TypedAccessor<NumComponents, ComponentType>(const Accessor &accessor)
			: accessor(accessor)
		{
		}
	};
}
#endif /* TYPEDACCESSOR_H */
