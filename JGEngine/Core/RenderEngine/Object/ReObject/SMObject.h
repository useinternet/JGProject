#pragma once


#include "Object/ReObject.h"



namespace RE
{
	class ReMesh;
	class GraphicsPipelineState;
	class SCStruct;
	class SDStructuredBuffer;
	class SDConstantBuffer;
	class RENDERENGINE_API SMObject : public ReObject
	{

	public:
		SMObject() : ReObject("SMObject") { }
		SMObject(const std::string& name) : ReObject(name) {}
	private:
	};

	class RENDERENGINE_API SMGBufferObject : public SMObject
	{
	public:
		SMGBufferObject() : SMObject("SMGBufferObject") {}
		SMGBufferObject(const std::string& name) : SMObject(name) {}

		void ConnectObjectStruct(const SCStruct& _struct);
		void ConnectSkinnedStruct(const SCStruct& _struct);
		template<typename T>
		uint32_t AddInstance(const T& data)
		{
			if (m_ConnectedObjectStruct->GetSize() != sizeof(T))
			{
				RE_LOG_ERROR("Failed Add Instance in {0}", GetName());
				return;
			}
			return m_ObjectSBuffer->AddData(data);
		}
		void RemoveInstance(uint32_t id);

		template<typename T>
		void SetInstance(uint32_t id, const T& data)
		{
			m_ObjectSBuffer->SetDataByID(id, data);
		}
	private:
		std::shared_ptr<ReMesh> m_Mesh;
		std::shared_ptr<GraphicsPipelineState> m_PSO;
	    std::shared_ptr<SCStruct> m_ConnectedObjectStruct;
		std::shared_ptr<SCStruct> m_ConnectedSkinnedStruct;
		std::shared_ptr<SDStructuredBuffer> m_ObjectSBuffer;
		std::shared_ptr<SDConstantBuffer>   m_SkinnedCBuffer;

		/*
		만약 Mesh, PSO, Object, Skinned 가 같다면 인스턴스 카운트를 하나 올린다.
		*/
		// Object -> 
		// Skinned
		// Material -> ConstantBuffer


		// 1. Data < - > Module data Name
        //
        // Mesh, SkeletalMesh
        // PSO
        // Instance
        // 
	};
	class RENDERENGINE_API SMLightObject : public SMObject
	{
	public:
		SMLightObject() : SMObject("SMLightObject") {}
		SMLightObject(const std::string& name) : SMObject(name) {}
	};
	class RENDERENGINE_API SMComputeObject : public SMObject
	{
	public:
		SMComputeObject() : SMObject("SMComputeObject") {}
		SMComputeObject(const std::string& name) : SMObject(name) {}

	};
}