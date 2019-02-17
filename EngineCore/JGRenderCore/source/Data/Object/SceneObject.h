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
		ObjectData*  m_Data        = nullptr;
		SkinnedData* m_SkinnedData = nullptr;
		SceneObject*    m_Parent = nullptr;
		std::vector<SceneObject*> m_Child;
		JGMaterial*     m_Material        = nullptr;
		MatPersonalData* m_MatPersonalData = nullptr;
		JGSkeletalMesh* m_SkeletalMesh = nullptr;
		JGAnimation*    m_Animation    = nullptr;
		std::shared_ptr<JGAnimationHelper> m_AnimHelper;
		JGLibrary::CoordinationController m_CoordinateSystem;
		JGLibrary::Transform m_WorldMatrix;
		JGLibrary::Transform m_TexMatrix;
	public:
		SceneObject(EObjectType type, const std::string& name);
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
		JGLibrary::Transform GetTransform();
		JGLibrary::Transform GetTexTransform();
	public:
		JGLibrary::Location& GetLocation() { return m_CoordinateSystem._Location; }
		JGLibrary::Rotation& GetRotation() { return m_CoordinateSystem._Rotation; }
		JGLibrary::Scale&    GetScale()    { return m_CoordinateSystem._Scale; }
	};
}