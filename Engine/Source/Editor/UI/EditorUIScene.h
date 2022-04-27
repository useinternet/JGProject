#pragma once
#include "JGCore.h"

namespace JG
{
	namespace Graphics
	{
		class Scene;
		class StaticRenderObject;
		class SkeletalRenderObject;
		class SceneObject;
	}

	ENUM_FLAG(EEditorUISceneFlags)
	enum class EEditorUISceneFlags
	{
		None = 0
	};


	struct EditorUISceneConfig
	{
		JVector2 Resolution;

		SharedPtr<Graphics::StaticRenderObject> mSkyBox;

		SharedPtr<Asset<IMesh>>     mMeshAsset = nullptr;
		SharedPtr<Asset<IMaterial>> mMaterialAsset = nullptr;
		

		const f32 mFarZ = 1000000.0f;
		const f32 mNearZ = 1.0f;
		JVector3 mEyePos = JVector3(0, 0, -200.0f);

	};
	class EditorUIScene
	{
		SharedPtr<ITexture>		    mSceneTexture;
		Graphics::Scene* mScene = nullptr;
	public:
		JVector2 mMousePosition;
		JVector2 mPrevMousePosition;

	};
}