#pragma once


#include "UI/UIView.h"


namespace JG
{
	class AssetFinderInitData
	{

	};

	class AssetFinderContextView : public ContextUIView<AssetFinderInitData>
	{
		JGCLASS
	public:
		virtual ~AssetFinderContextView() = default;
	protected:
		virtual void Initialize(const AssetFinderInitData& data) override;
		virtual void Destroy() override;
		virtual bool OnGUI() override;
	};
}
