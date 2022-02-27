#ifndef TYPEDACCESSOR_H
#define TYPEDACCESSOR_H

#include "gltf.h"

namespace Boiler { namespace gltf
{
	template<typename ComponentType, unsigned short NumComponents>
	class TypedAccessor
	{
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
				unsigned int stride = bufferView.byteStride.has_value()
					? bufferView.byteStride.value()
					: sizeof(ComponentType) * NumComponents;

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
		const Accessor &accessor;

		TypedAccessor(const Accessor &accessor, const BufferView &bufferView, const std::vector<std::byte> &data)
			: accessor(accessor), bufferView(bufferView), data(data)
		{
		}

		const ComponentType *operator[](int index) const
		{
			return *TypedIterator(index, data, accessor, bufferView);
		}

		TypedIterator begin() const
		{
			return TypedIterator(0, data, accessor, bufferView);
		}

		TypedIterator end() const
		{
			bufferView.byteStride.has_value() ? bufferView.byteStride.value()
				: sizeof(ComponentType) * NumComponents;

			unsigned int last = accessor.count;
			return TypedIterator(last, data, accessor, bufferView);
		}

		size_t size() const { return accessor.count; }
	};
}}

#endif /* TYPEDACCESSOR_H */
