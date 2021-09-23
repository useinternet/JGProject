#include "pch.h"
#include "SystemLayerStack.h"




namespace JG
{
	SystemLayerStack::~SystemLayerStack()
	{
		for(auto& layer : mLayers)
		{
			layer->OnDetach();
		}
	}

	void SystemLayerStack::PushLayer(const SharedPtr<ISystemLayer>& layer)
	{
		mLayers.insert(mLayers.begin() + mLayerInsertIndex, layer);
		++mLayerInsertIndex;

		layer->OnAttach();
	}

	void SystemLayerStack::PushOverlay(const SharedPtr<ISystemLayer>& layer)
	{
		mLayers.emplace_back(layer);

		layer->OnAttach();
	}

	void SystemLayerStack::PopLayer()
	{
		SharedPtr<ISystemLayer> layer;
		auto it = std::find(mLayers.begin(), mLayers.begin() + mLayerInsertIndex, layer);

		if(it != mLayers.begin() + mLayerInsertIndex)
		{
			layer->OnDetach();
			mLayers.erase(it);
			--mLayerInsertIndex;
		}
		
	}

	void SystemLayerStack::PopOverlay()
	{
		SharedPtr<ISystemLayer> layer;
		auto it = std::find(mLayers.begin() + mLayerInsertIndex, mLayers.end(), layer);

		if(it != mLayers.end())
		{
			layer->OnDetach();
			mLayers.erase(it);
		}
	}

	void SystemLayerStack::ForEach(const std::function<void(ISystemLayer*)>& action)
	{
		for(auto& layer : mLayers)
		{
			action(layer.get());
		}
	}
}
