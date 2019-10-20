#include "pch.h"
#include "SMObject.h"
#include "ReMesh.h"
#include "Object/DxObject/PipelineState.h"
#include "ShaderCode.h"
using namespace std;

namespace RE
{
	// Light Depth //

	// 상수 오브젝트
	// 그래픽 오브젝트
	// 라이트 오브젝트
	// 패쓰 오브젝트 (각종 필요한 변수넣기 -> 기본 적으로 ConstantBuffer) <- 모든 모듈에 들어감

	// 입력( 머터리얼, ConstantBuffer, StructuredBuffer, 텍스쳐)
	// 모듈
	// 결과물 (텍스쳐)
	void SMGBufferObject::ConnectObjectStruct(const SCStruct& _struct)
	{
		m_ConnectedObjectStruct = make_shared<SCStruct>(_struct);
		m_ObjectSBuffer = make_shared<SDStructuredBuffer>();
		m_ObjectSBuffer->Set(GetName() + "ObjectData", _struct, ShaderEnum::Visible_Vertex);
	}
	void SMGBufferObject::ConnectSkinnedStruct(const SCStruct& _struct)
	{
		m_ConnectedSkinnedStruct = make_shared<SCStruct>(_struct);
		m_SkinnedCBuffer = make_shared<SDConstantBuffer>();
		m_SkinnedCBuffer->Set(GetName() + "SkinnedData", ShaderEnum::Visible_Vertex);
	}

	void SMGBufferObject::RemoveInstance(uint32_t id)
	{
		m_ObjectSBuffer->RemoveData(id);
	}

	// 모듈 종류
	// GBuffer모듈 -> 그래픽 오브젝트를 입력으로 받는다.
	// 씬 모듈 -> 기본적으로 씬 오브젝트를 입력으로 받는다.
	// Light모둘 -> 라이트 오브젝트를 입력으로 받는다.
}