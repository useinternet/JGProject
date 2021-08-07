#pragma once

#include "Common/Define.h"
#include "SystemLayer.h"
#include <vector>
#include <functional>


namespace JG
{
	class SystemLayerStack
	{
	private:
		List<SharedPtr<ISystemLayer>> mLayers;
		u32 mLayerInsertIndex = 0;
	public:
		SystemLayerStack() = default;
		~SystemLayerStack();
	public:
		void PushLayer(const SharedPtr<ISystemLayer>& layer);
		void PushOverlay(const SharedPtr<ISystemLayer>& layer);


		void PopLayer();
		void PopOverlay();
		
		void ForEach(const std::function<void(ISystemLayer*)>& action);
	};
}

