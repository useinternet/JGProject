#pragma once
#include"Common/dllExportDefine.h"
namespace JGRC
{
	class JGDevice;
	/*
	EnumClass : EJGBufferType
	@e ConstantBuffer :  상수 버퍼
	@e VertexBuffer   :  정점 버퍼
	@e IndexBuffer    :  인덱스 버퍼
	@e InstanceBuffer :  인스턴스 버퍼
	*/
	enum class CORE_EXPORT EBufferType
	{
		ConstantBuffer = 0,
		VertexBuffer = 1,
		IndexBuffer = 2,
		InstanceBuffer = 3
	};
	/*
	EnumClass : EJGUsageType
	@e Static  : 정적인 타입
	@e Dynamic : 동적인 타입
	*/
	enum class CORE_EXPORT EUsageType
	{
		Static = 0,
		Dynamic = 1
	};
	/*
	EnumClass : EJGCPUType
	@e Access_Write : 쓰기로 접근
	@e Access_Read  : 읽기로 접근
	@e None         : 접근X
	*/
	enum class CORE_EXPORT ECPUType
	{
		Access_Write = 0,
		Access_Read = 1,
		None = 2
	};
	/*
	EnumClass : EJGMapType
	@e Read  :  읽기 허용
	@e Write :  쓰기 허용
	@e Read_Write : 읽기 쓰기 모두 허용
	@e Write_Discard : 쓰기 허용 ( 전의 데이터를 모두 지운다.)
	@e Write_NoOverWrite : 쓰기 허용( 전의 데이터를 지우지 않는다.)
	*/
	enum class CORE_EXPORT EMapType
	{
		Read = 0,
		Write = 1,
		Read_Write = 2,
		Write_Discard = 3,
		Write_NoOverWrite = 4
	};
}