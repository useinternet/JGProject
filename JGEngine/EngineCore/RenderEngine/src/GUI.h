#pragma once
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

class Event;

namespace RE
{
	class RenderDevice;
	class GUI
	{
	public:
		GUI() = default;
		virtual ~GUI() = default;
	public:
		virtual void Init(RenderDevice* renderDevice) = 0;
		virtual void Destroy() = 0;
		virtual void Render(ID3D12GraphicsCommandList* cmdList) = 0;
		virtual void OnEvent(Event& e) = 0;
	};
}
