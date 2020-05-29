#ifndef TYPEDACCESSOR_H
#define TYPEDACCESSOR_H

#include "gltf.h"

namespace gltf
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
			const BufferView &bufferView;

			size_t offset()
			{
				return position * bufferView.byteStride.value();
			}

		public:
			TypedIterator(size_t position, const std::vector<std::byte> &data,
						  const BufferView &bufferView) : data(data), bufferView(bufferView)
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

			ComponentType operator[](size_t index) const
			{
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
			return TypedIterator(0, data, bufferView);
		}

		TypedIterator end() const
		{
			unsigned int last = bufferView.byteLength.value() / bufferView.byteStride.value();
			return TypedIterator(last, data, bufferView);
		}

	};
}
#endif /* TYPEDACCESSOR_H */
