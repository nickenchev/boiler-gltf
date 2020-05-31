#ifndef TYPEDACCESSOR_H
#define TYPEDACCESSOR_H

#include "gltf.h"

namespace boiler { namespace gltf
{
	template<typename ComponentType>
	class TypedAccessor
	{
		const Accessor &accessor;
		const BufferView &bufferView;
		const std::vector<std::byte> &data;

		class TypedIterator
		{
			size_t position;
			const std::vector<std::byte> &data;
			const Accessor &accessor;
			const BufferView &bufferView;

			inline size_t offset()
			{
				int stride = bufferView.byteStride.has_value()
					? bufferView.byteStride.value()
					: sizeof(ComponentType);

				size_t offset = position * stride + (accessor.byteOffset + bufferView.byteOffset);
				return offset;
			}

		public:
			TypedIterator(size_t position, const std::vector<std::byte> &data,
						  const Accessor &accessor, const BufferView &bufferView)
				: data(data), accessor(accessor), bufferView(bufferView)
			{
				this->position = position;
			}

			TypedIterator &operator++()
			{
				++position;
				return *this;
			}

			bool operator!=(const TypedIterator &iterator) const
			{
				return this->position != iterator.position;
			}

			const ComponentType *operator*()
			{
				return (reinterpret_cast<const ComponentType *>(data.data() + offset()));
			}
		};

	public:
		TypedAccessor<ComponentType>(const Accessor &accessor,
									 const BufferView &bufferView,
									 const std::vector<std::byte> &data)
			: accessor(accessor), bufferView(bufferView), data(data)
		{
		}

		TypedIterator begin() const
		{
			return TypedIterator(0, data, accessor, bufferView);
		}

		TypedIterator end() const
		{
			int stride = bufferView.byteStride.has_value()
				? bufferView.byteStride.value()
				: sizeof(ComponentType);

			unsigned int last = accessor.count;
			return TypedIterator(last, data, accessor, bufferView);
		}

	};
}}

#endif /* TYPEDACCESSOR_H */
