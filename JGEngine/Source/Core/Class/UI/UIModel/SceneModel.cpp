#include "pch.h"
#include "Class/UI/UIModel/SceneModel.h"


namespace JG
{
	void SceneModel::Initialize()
	{

	}
	void SceneModel::Destroy()
	{
	}

	void SceneModel::SetMinSize(const JVector2& size)
	{
		mMinSize = size;
	}
	const JVector2& SceneModel::GetMinSize() const
	{
		return mMinSize;
	}
	void SceneModel::SetSceneTexture(SharedPtr<ITexture> texture)
	{
		mSceneTexture = texture;
	}

	SharedPtr<ITexture> SceneModel::GetSceneTexture() const
	{
		return mSceneTexture;
	}

}