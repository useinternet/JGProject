#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"ShaderObject.h"

namespace JGRC
{
	typedef UINT CompileFlag;
	class InputLayoutData;
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
		ShaderCompiler(const std::wstring& hlslPath,ShaderObject* object, InputLayoutData* data );
		~ShaderCompiler();
	private:
		bool Compile(const std::wstring& hlslPath, ShaderObject* object, InputLayoutData* data);
	private:
		static void ShaderInitialSetting(HWND hWnd, const std::string& functionName, const std::string& shaderVersion,
			const CompileFlag flag);
	};
}


