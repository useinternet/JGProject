#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"MaterialSystem/Data/ShaderData.h"
namespace JGRC
{
	class CORE_EXPORT ShaderObject
	{
	public:
		ID3D11InputLayout* InputLayout = nullptr;
		EShaderType Type;
		void* Buffer;
	};
}