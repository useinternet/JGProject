#pragma once
#include "Object/ReObject.h"


namespace RE
{
	class RenderDevice;
	class RENDERENGINE_API DxObject : public ReObject
	{
		friend RenderDevice;
	public:
		DxObject();
		DxObject(const std::string& name);
	protected:
		ID3D12Device* GetD3DDevice();
		ID3D12Device* GetD3DDevice() const;
	};
}