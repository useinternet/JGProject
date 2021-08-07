#pragma once
#include "JGCore.h"
#include "Graphics/Resource.h"
#include "Class/UI/UIModel.h"

namespace JG
{
	class Camera;
	class SceneModel : public UIModel
	{
		SharedPtr<ITexture> mSceneTexture;
		JVector2 mMinSize;
	protected:
		virtual void Initialize() override;
		virtual void Destroy() override;
	public:
		void SetMinSize(const JVector2& size);
		const JVector2& GetMinSize() const;


		void SetSceneTexture(SharedPtr<ITexture> texture);
		SharedPtr<ITexture> GetSceneTexture() const;
	public:
		virtual ~SceneModel() = default;
	};
}
