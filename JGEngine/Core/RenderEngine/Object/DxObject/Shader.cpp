#include "pch.h"
#include "Shader.h"

using namespace std;


namespace RE
{
	std::shared_ptr<Shader> Shader::MakeShader(ShaderType type)
	{
		switch (type)
		{
		case ShaderType::Vertex:
			return make_shared<VertexShader>();
		case ShaderType::Domain:
			return make_shared<DomainShader>();
		case ShaderType::Hull:
			return make_shared<HullShader>();
		case ShaderType::Geometry:
			return make_shared<GeometryShader>();
		case ShaderType::Pixel:
			return make_shared<PixelShader>();
		case ShaderType::Compute:
			return make_shared<ComputeShader>();
		}
		return nullptr;
	}
	Shader::Shader() : DxObject("Shader")
	{

	}
	Shader::Shader(const std::string& name) : DxObject(name)
	{
		m_ShaderCode = "None";
	}
	void Shader::Set(const std::string& shader_contents)
	{
		m_ShaderCode = move(shader_contents);
	}
	const std::string& Shader::GetErrorCode() const
	{
		return m_ErrorCode;
	}
	Microsoft::WRL::ComPtr<ID3DBlob> Shader::GetShaderCode() const
	{
		return m_CompiledShaderCode;
	}
	std::string Shader::ToString() const
	{
		return m_ShaderCode;
	}
	bool Shader::IsVaild() const
	{
		return m_CompiledShaderCode != nullptr;
	}

}