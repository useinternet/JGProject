#include"ShaderCompiler.h"
#include"DirectX/DirectX.h"
#include"JGRenderCore.h"
#include"InputLayout.h"
using namespace JGRC;
using namespace std;
HWND ShaderCompiler::hWnd;
string ShaderCompiler::FunctionName  = "main";
string ShaderCompiler::VSVersion     = "vs_5_0";
string ShaderCompiler::PSVersion     = "ps_5_0";
CompileFlag  ShaderCompiler::Flags   = D3D10_SHADER_ENABLE_STRICTNESS;
DirectX* ShaderCompiler::m_pDirectX = nullptr;
ShaderCompiler::ShaderCompiler(const std::wstring& hlslPath, ShaderObject* object)
{
	m_pDirectX = DirectX::GetInstance();
	Success = Compile(hlslPath, object);
}
ShaderCompiler::~ShaderCompiler()
{

}
bool ShaderCompiler::Compile(const std::wstring& hlslPath, ShaderObject* object)
{
	ID3D10Blob* Shader = nullptr;
	ID3D10Blob* Error  = nullptr;
	string version;
	switch (object->ShaderType)
	{
	case EShaderType::Pixel:
		version = PSVersion;
		break;
	case EShaderType::Vertex:
		version = VSVersion;
		break;
	}

	HRESULT result = S_OK;
	result = D3DX11CompileFromFile(hlslPath.c_str(), nullptr, nullptr,
		FunctionName.c_str(), version.c_str(), Flags, 0, nullptr, &Shader, &Error, nullptr);
	if (FAILED(result))
	{
		char* compileErrors;
		if (Error)
		{
			compileErrors = (char*)(Error->GetBufferPointer());
		}
		else
		{
			MessageBox(hWnd, hlslPath.c_str(), TT("Missing Shader File"), MB_OK);
		}
		return false;
	}
	switch (object->ShaderType)
	{
	case EShaderType::Vertex:
		result = m_pDirectX->GetDevice()->CreateVertexShader( Shader->GetBufferPointer(), Shader->GetBufferSize(),
			nullptr, (ID3D11VertexShader**)&(object->ShaderBuffer));
		m_pDirectX->GetDevice()->CreateInputLayout(InputLayout::Get(), InputLayout::Size(),
			Shader->GetBufferPointer(), Shader->GetBufferSize(), &object->Layout);
		break;
	case EShaderType::Pixel:
		result = m_pDirectX->GetDevice()->CreatePixelShader(Shader->GetBufferPointer(), Shader->GetBufferSize(),
			nullptr, (ID3D11PixelShader**)&(object->ShaderBuffer));
		break;
	default:
		// 아직 미구현
		break;
	}
	if (FAILED(result))
	{
		return false;
	}
	return true;
}


void ShaderCompiler::ShaderInitialSetting(HWND hwnd, const string& functionName, const string& shaderVersion,
	const CompileFlag flag)
{
	hWnd = hwnd;
	FunctionName  = functionName;
	VSVersion     = "vs_" + shaderVersion;
	PSVersion     = "ps_" + shaderVersion;
	Flags = flag;
}