#include"JGShaderCompiler.h"
#include"../JGDeviceD.h"
#include"../../EngineStatics/JGLog.h"

JGShaderCompiler::JGShaderCompiler(ID3D11VertexShader** VertexShader, ID3D11PixelShader** PixelShader,
	D3D11_INPUT_ELEMENT_DESC* elementDesc, UINT elementSize, ID3D11InputLayout** InputLayout,
	const std::string& entryPoint, const std::string& Version) :
	pVertexShaderAddress(VertexShader), pPixelShaderAddress(PixelShader),
	pElementDesc(elementDesc), ElementSize(elementSize), pInputLayoutAddress(InputLayout),
	EntryPoint(entryPoint), VSVersion("vs_" + Version), PSVersion("ps_" + Version)
{


}
JGShaderCompiler::~JGShaderCompiler()
{

}


bool JGShaderCompiler::Compile(JGDeviceD* Device, const wchar_t* VertexHlslPath, const wchar_t* PixelHlslPath)
{
	HRESULT result = S_OK;
	// 버텍스 셰이더 컴파일 및 생성
	result = D3DX11CompileFromFile(VertexHlslPath, nullptr, nullptr, EntryPoint.c_str(), VSVersion.c_str(),
		D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, m_VertexShaderBuffer.GetAddressOf(),
		nullptr, nullptr);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Compile of (%s) Failed."), VertexHlslPath);
		return false;
	}
	result = Device->GetDevice()->CreateVertexShader(m_VertexShaderBuffer->GetBufferPointer(),
		m_VertexShaderBuffer->GetBufferSize(), nullptr, pVertexShaderAddress);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create VertexShader of (%s) Failed."), VertexHlslPath);
		return false;
	}
	// 픽셀 셰이더 컴파일 및 생성
	result = D3DX11CompileFromFile(PixelHlslPath, nullptr, nullptr, EntryPoint.c_str(), PSVersion.c_str(),
		D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, m_PixelShaderBuffer.GetAddressOf(), nullptr, nullptr);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Compile of (%s) Failed."), PixelHlslPath);
		return false;
	}
	result = Device->GetDevice()->CreatePixelShader(m_PixelShaderBuffer->GetBufferPointer(), m_PixelShaderBuffer->GetBufferSize(),
		nullptr, pPixelShaderAddress);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create PixelShader of (%s) Failed."), PixelHlslPath);
		return false;
	}

	// 레이아웃 생성
	result = Device->GetDevice()->CreateInputLayout(pElementDesc, ElementSize,
		m_VertexShaderBuffer->GetBufferPointer(), m_VertexShaderBuffer->GetBufferSize(), pInputLayoutAddress);
	if (FAILED(result))
	{
		JGLog::Write(ELogLevel::Error, TT("Create Layout of (%s) Failed."), VertexHlslPath);
		return false;
	}

	return true;
}