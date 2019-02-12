#pragma once
#include"Object.h"




namespace JGRC
{
	class JGMaterial;
	class JGSkeletalMesh;
	class JGAnimation;
	class JGAnimationHelper;
	class RCORE_EXPORT SceneObject : public Object
	{
	private:
		static UINT Count;
		static UINT SkinnedIndex;
	private:
		UINT m_ObjCBIndex;
		UINT m_SkinnedCBIndex;
		SceneObject*    m_Owner        = nullptr;
		JGMaterial*     m_Material     = nullptr;
		JGSkeletalMesh* m_SkeletalMesh = nullptr;
		JGAnimation*    m_Animation    = nullptr;
		std::shared_ptr<JGAnimationHelper> m_AnimHelper;
		DirectX::XMFLOAT4X4 m_World        = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_TexTransform = MathHelper::Identity4x4();

		DirectX::XMFLOAT3 m_Location = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 m_Rotation = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 m_Scale    = { 1.0f,1.0f,1.0f };
	public:
		SceneObject(UINT index, EObjectType type, const std::string& name);
		virtual ~SceneObject() = default;
		virtual void Build(ID3D12GraphicsCommandList* CommandList) override;
		virtual void Update(const GameTimer& gt, FrameResource* CurrFrameResource) override;
		virtual void Draw(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList, EObjectRenderMode Mode) override;

	};
}