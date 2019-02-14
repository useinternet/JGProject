#pragma once
#include"Object.h"




namespace JGRC
{
	class JGMaterial;
	struct MatPersonalData;
	class JGSkeletalMesh;
	class JGAnimation;
	class JGAnimationHelper;
	class RCORE_EXPORT SceneObject : public Object
	{
	private:
		static UINT Count;
	private:
		UINT m_ObjCBIndex;
		UINT m_SkinnedCBIndex;
		SceneObject*    m_Parent = nullptr;
		std::vector<SceneObject*> m_Child;
		JGMaterial*     m_Material        = nullptr;
		MatPersonalData* m_MatPersonalData = nullptr;
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
	public:
		void AttachTo(SceneObject* obj);
		SceneObject* GetChild(UINT idx) const;
		SceneObject* GetParent() const;
	public:
		JGMaterial* GetMaterial() const;
		void SetMaterial(JGMaterial* mat, const std::string& matDataName);
		void SetAnimation(const std::string& name);
	public:
		const DirectX::XMFLOAT3&   GetLocation() const { return m_Location; }
		const DirectX::XMFLOAT3&   GetRotation() const { return m_Rotation; }
		const DirectX::XMFLOAT3&   GetScale()    const { return m_Scale; }
		const DirectX::XMFLOAT4X4  GetWorld()    const;
		const DirectX::XMFLOAT4X4& GetTex()      const { return m_TexTransform; }
	public:
		void SetLocation(float x, float y, float z);
		void SetRotation(float pitch, float yaw, float roll);
		void SetScale(float x, float y, float z);
		void SetScale(float x);
	public:
		void OffsetLocation(float x, float y, float z);
		void OffsetRotation(float pitch, float yaw, float roll);
		void OffsetScale(float x, float y, float z);
		void OffsetScale(float x);
	private:
		void UpdateWorldMatrix();
	};
}