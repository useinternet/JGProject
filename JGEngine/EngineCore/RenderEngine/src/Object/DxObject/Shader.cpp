#include "pch.h"
#include "Shader.h"

using namespace std;


namespace RE
{
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