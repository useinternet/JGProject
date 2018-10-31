#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"ShaderObject.h"

// D3D10_SHADER_MACRO - > 보류   D3DX11CompileFromFile의 두번째 인자.
// ID3D10Include - > 보류  D3DX11CompileFromFile 세번째 인자.
// ID3DX11ThreadPump -> 안쓸거같다. 일단 보류




// ID3D11ClassLinkage -> 이것도 보류
namespace JGRC
{
	typedef UINT CompileFlag;
	class InputLayout;

	class ShaderCompiler
	{
	private:
		friend class JGRenderCore;
	public:
		static HWND hWnd;
		static std::string FunctionName;
		static std::string VSVersion;
		static std::string PSVersion;
		static CompileFlag  Flags;
	private:
		static class DirectX* m_pDirectX;
	public:
		bool Success = false;
	public:
		ShaderCompiler() {}
		ShaderCompiler(const std::wstring& hlslPath,ShaderObject* object, InputLayout* layout);
		~ShaderCompiler();
	private:
		bool Compile(const std::wstring& hlslPath, ShaderObject* object, InputLayout* layout);
	private:
		static void ShaderInitialSetting(HWND hWnd, const std::string& functionName, const std::string& shaderVersion,
			const CompileFlag flag);
	};
}


