#include "pch.h"
#include "FrameBuffer.h"
#include "Application.h"
#include "Graphics/GraphicsAPI.h"
#include "Platform/Graphics/DirectX12/DirectX12FrameBuffer.h"

namespace JG
{
	SharedPtr<IFrameBuffer> IFrameBuffer::Create(const FrameBufferInfo& info)
	{
		JGASSERT_IF(info.Handle != 0, TT("Data is Null"));
		auto api = Application::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		return api->CreateFrameBuffer(info);
	}
}