#pragma once
#include"Resource.h"


namespace Dx12
{
	class Buffer : public Resource
	{
	private:

	public:
		Buffer(const std::string& name = "Buffer");

		Buffer(
			const D3D12_RESOURCE_DESC& Desc,
			uint32_t numElements,
			uint32_t elementSize,
			const std::string& name = "Buffer");

		virtual ~Buffer();
		virtual void CreateViews(uint32_t numElements, uint32_t elementSize) = 0;
	};
}