#pragma once
#include"DxCommon/DxCommon.h"
#include"ObjectState.h"
namespace JGRC
{
	class RCORE_EXPORT Object
	{
		struct ObjState
		{
			bool Init = false;
			bool Visible = true;
			bool Active = true;
			bool Dead = false;
		};
	private:
		std::string   m_Name;
		EObjectType   m_Type;
		CommonPSOPack m_PSOPack;
		ObjState      m_State;
		int m_UpdateNotify = CPU_FRAMERESOURCE_NUM;


		JGBaseMesh*   m_Mesh;
		std::string   m_MeshName;

		D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	public:
		Object(EObjectType type, const std::string& name);
		virtual ~Object() = default;
		virtual void Build(ID3D12GraphicsCommandList* CommandList) {}
		virtual void Update(const GameTimer& gt, FrameResource* CurrFrameResource) {}
		virtual void Draw(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList, EObjectRenderMode mode) {}
	public:
		void SetMesh(JGBaseMesh* mesh, const std::string& meshName);
		JGBaseMesh* GetMesh();
	public:
		EObjectType          GetType()      const { return m_Type; }
		const std::string&   GetName()      const { return m_Name; }
		const std::string&   GetMeshName()  const { return m_MeshName; }
		const CommonPSOPack& GetPSOPack_c() const { return m_PSOPack; }
		const ObjState&      GetState_c()   const { return m_State; }   
	
		D3D12_PRIMITIVE_TOPOLOGY GetPrimitive() const { return m_PrimitiveTopology; }
	public:
		void UnVisible() { m_State.Visible = false; }
		void Visible()   { m_State.Visible = true; }
		bool IsVisible() { return m_State.Visible; }
		void Active()    { m_State.Active = true; }
		void DeActive()  { m_State.Active = false; }
		bool IsActive()  { return m_State.Active; }
		void Revive()    { m_State.Dead = false; }
		void Dead()      { m_State.Dead = true; }
		bool IsDead()    { return m_State.Dead; }
	protected:
		void ClearNotify()    { m_UpdateNotify = CPU_FRAMERESOURCE_NUM; }
		void UpdatePerFrame() { m_UpdateNotify--; }
		bool IsCanUpdate()    { return m_UpdateNotify > 0; }
	protected:
		CommonPSOPack& GetPSOPack() { return m_PSOPack; }
		ObjState& GetState()        { return m_State; }
	protected:
		void SetType(EObjectType type) { m_Type = type; }
	};
}