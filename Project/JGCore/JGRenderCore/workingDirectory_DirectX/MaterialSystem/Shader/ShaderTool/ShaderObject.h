#pragma once
namespace JGRC
{
	enum class EShaderType
	{
		Pixel,
		Vertex,
		Header
	};
	typedef struct ShaderObject
	{
		void* ShaderBuffer;
		EShaderType ShaderType;
		ID3D11InputLayout* Layout;
	}ShaderObject;
}
