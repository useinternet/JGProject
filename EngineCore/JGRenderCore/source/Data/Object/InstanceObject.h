#pragma once
#include"Object.h"

namespace JGRC
{
	class JGMaterial;
	struct MatPersonalData;
	class RCORE_EXPORT InstanceObject : public Object
	{
	private:
		struct RCORE_EXPORT Unit
		{
			UINT  ID;
			InstanceData* Data;
			Unit* Parent = nullptr;
			std::vector<Unit*> Child;

			UINT InstanceCBIndex;
			UINT SkinnedCBIndex;
		
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 TexTransform;
			DirectX::XMFLOAT3   Location = { 0.0f,0.0f,0.0f };
			DirectX::XMFLOAT3   Rotation = { 0.0f,0.0f,0.0f };
			DirectX::XMFLOAT3   Scale    = { 0.0f,0.0f,0.0f };
			MatPersonalData*    MatData = nullptr;

			int UpdateNotify = CPU_FRAMERESOURCE_NUM;

			DirectX::XMFLOAT4X4 GetWorld()
			{
				if (Parent)
				{
					DirectX::XMMATRIX SimWorld       = DirectX::XMLoadFloat4x4(&World);
					DirectX::XMMATRIX ParentSimWorld = DirectX::XMLoadFloat4x4(&Parent->GetWorld());
					DirectX::XMMATRIX result         = DirectX::XMMatrixMultiply(SimWorld, ParentSimWorld);
					DirectX::XMFLOAT4X4 result4x4f;
					DirectX::XMStoreFloat4x4(&result4x4f, result);
					return result4x4f;
				}
				return World;
			}
			void ClearNotify()    { UpdateNotify = CPU_FRAMERESOURCE_NUM; }
			void UpdatePerFrame() { UpdateNotify--; }
			bool IsCanUpdate()    { return UpdateNotify > 0; }
		};
	private:
		std::vector<Unit> m_Unit;
		JGSkeletalMesh*   m_SkeletalMesh = nullptr;
		JGMaterial*       m_Material = nullptr;
		std::unordered_map<UINT, MatPersonalData*> m_UnitMatDatas;
		//
		InstanceObject* m_Parent = nullptr;
		std::vector<InstanceObject*> m_Child;

		bool m_bSkinnedObject = false;
	public:
		InstanceObject(EObjectType type, const std::string& name)
			: Object(type, name) {}
		virtual ~InstanceObject() = default;
		virtual void Build(ID3D12GraphicsCommandList* CommandList) override;
		virtual void Update(const GameTimer& gt, FrameResource* CurrFrameResource) override;
		virtual void Draw(
			FrameResource* CurrFrameResource, 
			ID3D12GraphicsCommandList* CommandList,
			EObjectRenderMode Mode) override;
	public:
		UINT AddInstanceUnit();
		void AttachTo(InstanceObject* obj);
		void SetMaterial(JGMaterial* mat);
		void SetMaterialData(UINT ID, const std::string& name);
		void SetMaterialData(UINT Start, UINT End, const std::string& name);
		JGMaterial* GetMaterial();
		MatPersonalData* GetMaterialData(UINT ID);
	public:
		Unit* GetUnit(UINT ID);
		const Unit& GetUnit_c(UINT ID) const;
		UINT  GetUnitCount();
	public:
		const DirectX::XMFLOAT3&   GetLocation(UINT ID) const;
		const DirectX::XMFLOAT3&   GetRotation(UINT ID) const;
		const DirectX::XMFLOAT3&   GetScale(UINT ID)    const;
		const DirectX::XMFLOAT4X4& GetWorld(UINT ID)    const;
		const DirectX::XMFLOAT4X4& GetTex(UINT ID)      const;
	public:
		void SetLocation(UINT ID, float x, float y, float z);
		void SetRotation(UINT ID, float pitch, float yaw, float roll);
		void SetScale(UINT ID, float x, float y, float z);
		void SetScale(UINT ID, float x);
	public:
		void OffsetLocation(UINT ID, float x, float y, float z);
		void OffsetRotation(UINT ID, float pitch, float yaw, float roll);
		void OffsetScale(UINT ID, float x, float y, float z);
		void OffsetScale(UINT ID, float x);
	private:
		void UpdateWorldMatrix(UINT ID);
	};
}