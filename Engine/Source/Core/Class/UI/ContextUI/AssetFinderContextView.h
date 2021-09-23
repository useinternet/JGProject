#pragma once


#include "Class/UI/UIView.h"


namespace JG
{
	class AssetFinderInitData
	{

	};

	class AssetFinderContextView : public ContextUIView<AssetFinderInitData>
	{
		JGCLASS
	protected:
		virtual void Initialize(const AssetFinderInitData& data) override;
		virtual void Destroy() override;
		virtual bool OnGUI() override;
	};
}
